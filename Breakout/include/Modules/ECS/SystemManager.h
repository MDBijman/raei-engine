#pragma once
#include "System.h"
#include "Modules/Memory/safe_queue.h"

#include <tuple>
#include <atomic>
#include <condition_variable>
#include <memory>

namespace ecs
{
	/*
	* System Manager
	*/
	template<class ECS>
	class base_system_manager;

	template<template<class ComponentTuple, class FilterTuple> class ECS, class... Components, class... Filters>
	class base_system_manager<ECS<std::tuple<Components...>, std::tuple<Filters...>>>
	{
		using ecs = ECS<std::tuple<Components...>, std::tuple<Filters...>>;
		using system = System<ecs>;

		class worker_thread
		{
			/*
			* Pointer to the ecs that contains the game world corresponding to the systems that are executed
			* by this worker. This pointer must be updated when the ecs is moved.
			*/
			ecs* parent_ecs;

			// False indicates the thread should shutdown as soon as it is woken.
			std::atomic_bool keep_running;

			/*
			* Used for waiting and waking this worker. This worker_thread locks the wakeup_mutex and calls
			* wakeup.wait(). The parent group locks wakeup_mutex, sets should_wake to true, and calls
			* wakeup.notify_all(). After being woken, this worker sets should_wake to false.
			*/
			std::condition_variable wakeup;

			/*
			* Necessary to coordinate using the 'wakeup' variable between this worker_thread and the parent group.
			*/
			std::mutex wakeup_mutex;

			/*
			* True if this thread should wake. This is used together with 'wakeup' to deal with spurious wakeups while
			* waiting for the parent group to wake this thread. I.e. when this worker is randomly woken by the OS
			* instead of by the parent group, should_wake will still be false, and the worker will go to sleep again.
			*/
			bool should_wake = false;

			/*
			* When a worker finishes it should decrement this, when workers_running is 0 all workers are done.
			* Shared between this thread, the parent group, and the other threads in the group.
			*/
			std::shared_ptr<std::atomic_int> workers_running;

			/*
			* The parent group pushes work in the form of a system, which is dequeued by a single worker that calls
			* update on the system. Shared between this thread, the parent group, and the other threads in the group.
			*/
			std::shared_ptr<memory::safe_queue<10, system*>> work_queue;

			// The thread object that runs the 'run' method of this worker_thread
			std::thread thread;

		public:
			worker_thread(std::shared_ptr<std::atomic_int> workers,
				std::shared_ptr<memory::safe_queue<10, system*>> queue, ecs* parent_ecs) :
				parent_ecs(parent_ecs), keep_running(true), workers_running(workers), work_queue(queue),
				thread(&worker_thread::run, this) {}

			/*
			* Updates the ecs used by this worker. The ecs is given to a system when its 'update' is called.
			*/
			void set_ecs(ecs* ecs)
			{
				this->parent_ecs = ecs;
			}

			/*
			* Kills the thread corresponding to this worker.
			*/
			void kill()
			{
				{
					std::scoped_lock<std::mutex> sl(wakeup_mutex);
					keep_running.store(false);
					should_wake = true;
				}
				wakeup.notify_all();
				thread.join();
			}

			/*
			* Wakes the thread corresponding to this worker.
			*/
			void wake()
			{
				{
					std::scoped_lock<std::mutex> thread_mutex(wakeup_mutex);
					should_wake = true;
				}
				wakeup.notify_all();
			}

		private:
			void run()
			{
				while (keep_running.load())
				{
					std::unique_lock<std::mutex> lk(wakeup_mutex);
					wakeup.wait(lk, [this]() { return should_wake; });
					should_wake = false;

					while (true)
					{
						auto work = work_queue->dequeue();
						if (!work.has_value())
							break;

						auto sys = work.value();
						sys->update(*parent_ecs);
					}

					(*workers_running)--;
				}
			}
		};


		class group
		{
			using group_work_queue = memory::safe_queue<10, system*>;

			std::array<std::unique_ptr<worker_thread>, 4> threads;

			std::shared_ptr<std::atomic_int> workers_running;
			std::shared_ptr<group_work_queue> work_queue;
			ecs* parent_ecs;

			std::vector<std::unique_ptr<system>> systems;

		public:
			group(ecs* parent_ecs) :
				workers_running(std::make_shared<std::atomic_int>()),
				parent_ecs(parent_ecs),
				work_queue(std::make_shared<group_work_queue>())
			{
				for (int i = 0; i < 4; i++)
					threads.at(i) = std::make_unique<worker_thread>(workers_running, work_queue, parent_ecs);
			}

			group(group&& o) :
				parent_ecs(o.parent_ecs),
				systems(std::move(o.systems)),
				workers_running(std::move(o.workers_running)),
				work_queue(std::move(o.work_queue)),
				threads(std::move(o.threads))
			{}

			~group()
			{
				for (auto& t : threads)
				{
					if (t) t->kill();
				}
			}

			void update()
			{
				if (systems.size() == 0) return;

				run_workers();
			}

			void add_system(std::unique_ptr<system>&& s)
			{
				systems.push_back(std::move(s));
			}

			void set_ecs(ecs* ecs)
			{
				this->parent_ecs = ecs;
				for (auto& t : threads)
					t->set_ecs(ecs);
			}

		private:
			void run_workers()
			{
				workers_running->store(4);

				// Put work in threads
				for (std::size_t i = 0; i < systems.size(); i++)
					work_queue->enqueue(systems.at(i).get());

				// Notify threads to run systems
				for (auto& t : threads)
					t->wake();

				// TODO Improve to reduce latency
				while (workers_running->load() != 0)
				{
					std::this_thread::yield();
				}
			}
		};

	public:
		using group_id = uint32_t;

		base_system_manager(ecs* parent_ecs) : parent_ecs(parent_ecs) {}

		group_id create_group()
		{
			counter++;
			system_groups.insert({ counter, group(parent_ecs) });
			order.push_back(counter);
			return counter;
		}

		group& get_group(group_id id)
		{
			return system_groups.find(id);
		}

		void add_to_group(group_id id, std::unique_ptr<system> sys)
		{
			system_groups.at(id).add_system(std::move(sys));
		}

		void update()
		{
			for (auto id : order)
			{
				system_groups.at(id).update();
			}
		}

		void stop_group(group_id id)
		{
			system_groups.at(id).should_stop.store(true);
			system_groups.at(id).thread.join();
		}

		void set_ecs(ecs* ecs)
		{
			this->parent_ecs = ecs;
			for (auto& group : system_groups)
				group.second.set_ecs(ecs);
		}

	private:
		ecs * parent_ecs;

		// Vector of system groups
		std::unordered_map<group_id, group> system_groups;
		std::vector<group_id> order;

		group_id counter = 0;
	};

}
