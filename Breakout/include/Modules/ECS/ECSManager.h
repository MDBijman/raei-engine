#pragma once
#include "Component.h"
#include "Entity.h"
#include "System.h"
#include "Modules/TemplateUtils/TypeIndex.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"
#include "Modules/Time/Timer.h"
#include "Modules/Memory/safe_queue.h"

#include <unordered_map>
#include <unordered_set>
#include <stdint.h>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <memory>

namespace ecs
{
	/*
	* Template Parameter helper classes
	*/

	// Holds a list of component types
	template<class...> struct filter
	{
		void addEntity(uint32_t e)
		{
			entities.insert(e);
		}

		bool hasEntity(uint32_t e)
		{
			return entities.find(e) != entities.end();
		}

		void removeEntity(uint32_t e)
		{
			entities.erase(e);
		}

		std::unordered_set<uint32_t> entities;
	};

	enum class option
	{
		defered,
		immediate,
	};

	/*
	* System Manager
	*/

	template<class ECS>
	class base_system_manager;

	template<template<class ComponentTuple, class FilterTuple> class ECS, class... Components, class... Filters>
	class base_system_manager<ECS<std::tuple<Components...>, std::tuple<Filters...>>>
	{
		using ecs = ECS<std::tuple<Components...>, std::tuple<Filters...>>;
		using system = System<std::tuple<Components...>, std::tuple<Filters...>>;

		class worker_thread
		{
			/*
			* Pointer to the ecs that contains the game world corresponding to the systems that are executed 
			* by this worker. This pointer must be updated when the ecs is moved. 
			*/
			ecs* parent_ecs;

			// Shared between this thread and the parent group, false indicates the thread should shutdown.
			std::shared_ptr<std::atomic_bool> keep_running;

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
			worker_thread(std::shared_ptr<std::atomic_bool> keep_running, std::shared_ptr<std::atomic_int> workers,
				std::shared_ptr<memory::safe_queue<10, system*>> queue, ecs* parent_ecs) :
				parent_ecs(parent_ecs), keep_running(keep_running), workers_running(workers), work_queue(queue),
				thread(&worker_thread::run, this) {}

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
			* Updates the ecs used by this worker. The ecs is given to a system when its 'update' is called.
			*/
			void set_ecs(ecs* ecs)
			{
				this->parent_ecs = ecs;
			}

		private:
			void run()
			{
				while (keep_running->load())
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
			std::array<std::unique_ptr<worker_thread>, 4> threads;

			std::shared_ptr<std::atomic_bool> keep_running;

			std::shared_ptr<std::atomic_int> workers_running;
			std::shared_ptr<group_work_queue> work_queue;
			ecs* parent_ecs;

			std::vector<std::unique_ptr<system>> systems;

		public:
			using group_work_queue = memory::safe_queue<10, system*>;

			group(ecs* parent_ecs) : keep_running(std::make_shared<std::atomic_bool>(true)),
				workers_running(std::make_shared<std::atomic_int>()),
				parent_ecs(parent_ecs), work_queue(std::make_shared<group_work_queue>())
			{
				for (int i = 0; i < 4; i++)
					threads.at(i) = std::make_unique<worker_thread>(keep_running, workers_running, work_queue, parent_ecs);
			}

			group(group&& o) : parent_ecs(o.parent_ecs),
				systems(std::move(o.systems)), workers_running(std::move(o.workers_running)),
				keep_running(std::move(o.keep_running)), work_queue(std::move(o.work_queue)),
				threads(std::move(o.threads))
			{}

			~group()
			{
				for (auto& t : threads)
					t->keep_running.store(false);

				run_workers();
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
				{
					{
						std::scoped_lock<std::mutex> thread_mutex(t->wakeup_mutex);
						t->should_wake = true;
					}
					t->wakeup.notify_all();
				}

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

	/*
	* Base manager
	*/

	template<typename ComponentTuple, typename FilterTuple>
	class base_manager;

	template<class... Components, class... Filters>
	class base_manager<std::tuple<Components...>, std::tuple<Filters...>>
	{
	private: // Type declarations and helper structs

		using component_tuple = std::tuple<Components...>;
		using filter_tuple = std::tuple<Filters...>;

		using system = System<component_tuple, filter_tuple>;
		using system_manager = base_system_manager<base_manager<component_tuple, filter_tuple>>;

		static constexpr int ComponentCount = sizeof...(Components);

		/*
		* Provides helper functionality for:
		* - locking mutexes corresponding to components of a filter through lock()
		* - checking if an entity contains all components of a filter through update()
		*/
		template<class Filter>
		struct filter_helper {};

		/*
		* Filter Helper specialization for filters with 2+ components
		*/
		template<class C1, class C2, class... Cs>
		struct filter_helper<filter<C1, C2, Cs...>>
		{
			static bool update(std::unordered_map<uint32_t, Entity<Components...>>& entities, uint32_t e)
			{
				return entities.at(e).template hasComponents<C1, C2, Cs...>();
			}

			static void lock(std::array<std::mutex, ComponentCount>& locks)
			{
				std::lock(
					locks.at(type_index<C1, Components...>::value),
					locks.at(type_index<C2, Components...>::value),
					locks.at(type_index<Cs, Components...>::value)...
				);
			}

			static void unlock(std::array<std::mutex, ComponentCount>& locks)
			{
				locks.at(type_index<C1, Components...>::value).unlock();
				locks.at(type_index<C2, Components...>::value).unlock();
				(locks.at(type_index<Cs, Components...>::value).unlock(), ...);
			}
		};

		/*
		* Filter Helper specialization for filters with a single component
		*/
		template<class C>
		struct filter_helper<filter<C>>
		{
			static bool update(std::unordered_map<uint32_t, Entity<Components...>>& entities, uint32_t e)
			{
				return entities.at(e).template hasComponents<C>();
			}

			// Fails when components size is one
			static void lock(std::array<std::mutex, ComponentCount>& locks)
			{
				locks.at(type_index<C, Components...>::value).lock();
			}

			static void unlock(std::array<std::mutex, ComponentCount>& locks)
			{
				locks.at(type_index<C, Components...>::value).unlock();
			}
		};

		/*
		* Filter Result is a helper for controlling component mutexes and filter results.
		* The struct contains the result of a filter operation, and the filter components are locked upon creation.
		* When the destructor is called, the mutexes are unlocked and become available for other threads.
		*/
		template<class... Components>
		struct filter_result {};

		template<class... Components>
		struct filter_result<filter<Components...>>
		{
			filter_result(const std::unordered_set<uint32_t>& es, base_manager<component_tuple, filter_tuple>& ecs) :
				entities(es),
				ecs(ecs)
			{
				filter_helper<filter<Components...>>::lock(ecs.component_mutexes);
			}
			~filter_result()
			{
				if (owns) filter_helper<filter<Components...>>::unlock(ecs.component_mutexes);
			}

			filter_result(const filter_result&) = delete;
			filter_result(filter_result&& o) : entities(o.entities), ecs(o.ecs), owns(true)
			{
				o.owns = false;
			}

			void operator=(const filter_result&) = delete;
			void operator=(filter_result&& o)
			{
				o.owns = false;
				owns = true;
				entities = o.entities;
				ecs = o.ecs;
			};

			const std::unordered_set<uint32_t>& entities;

		private:
			bool owns = true;
			base_manager<component_tuple, filter_tuple>& ecs;
		};

		template<class... Components>
		friend struct filter_result;

	private: // Fields

		/*
		* Containers
		*/
		system_manager child_system_manager;

		std::unordered_set<uint32_t> to_remove_entities;

		// Tuple for each filter type
		std::tuple<Filters...> filters;

		// Tuple of maps for each component type
		std::tuple<std::unordered_map<uint32_t, Components>...> components;

		// For each component a mutex
		std::mutex master_component_mutex;
		std::array<std::mutex, sizeof...(Components)> component_mutexes;

		// Map of entity ids to entities
		std::unordered_map<uint32_t, Entity<Components...>> entities;

		uint32_t entityCount = 0;

	public: // Methods
		base_manager() : child_system_manager(this) { }

		base_manager(base_manager&& o) :
			child_system_manager(std::move(o.child_system_manager)),
			to_remove_entities(std::move(o.to_remove_entities)),
			filters(std::move(o.filters)),
			components(std::move(o.components)),
			entities(std::move(o.entities)),
			entityCount(o.entityCount)
		{
			child_system_manager.set_ecs(this);
		}

		/* Entity Methods */

		uint32_t createEntity()
		{
			Entity<Components...> e = Entity<Components...>(++entityCount);
			entities.insert(std::make_pair(e.id, e));
			return entityCount;
		}

		void removeEntity(uint32_t e, option o = option::immediate)
		{
			if (o == option::defered)
			{
				to_remove_entities.insert(e);
			}
			else
			{
				removeComponents<Components...>(e);
				updateFilters<Filters...>(e);
				entities.erase(e);
			}
		}

		/* Component Methods */

		/*
		* Returns ComponentType of the given entity.
		*/
		template<class ComponentType>
		ComponentType& getComponent(uint32_t e)
		{
			constexpr auto index = type_index<ComponentType, Components...>::value;
			return std::get<index>(components).at(e);
		}

		/*
		* Sets the parent of the given component and adds it to the vector of components of that type.
		*/
		template<class ComponentType>
		std::enable_if_t<std::is_base_of_v<Component, ComponentType>, ComponentType&> addComponent(uint32_t entityId,
			ComponentType&& c)
		{
			// Set the parent of the component
			c.parent = entityId;

			// Index of ComponentType in the list of component types
			constexpr size_t index = type_index<ComponentType, Components...>::value;

			// Add the component to the vector of components of that type
			std::get<index>(components).insert(std::make_pair(entityId, std::forward<ComponentType>(c)));

			// Add the component type to the bitfield of the entity
			entities.at(entityId).template addComponent<ComponentType>();

			updateFilters<Filters...>(entityId);

			return std::get<index>(components).find(entityId)->second;
		}

		/*
		* Returns all entities that match the signature.
		*/
		template<class Filter>
		//std::pair<filter_lock<Filter>, const std::unordered_set<uint32_t>&> filterEntities()
		filter_result<Filter> filterEntities()
		{
			// We can only create components that derive from Component class
			constexpr size_t index = type_index<Filter, Filters...>::value;

			return filter_result<Filter>(std::get<index>(filters).entities, *this);
		}

		system_manager& get_system_manager()
		{
			return this->child_system_manager;
		}

		/*
		* Runs all system groups and then executes defered entity removals.
		*/
		void update()
		{
			this->child_system_manager.update();

			for (auto e : to_remove_entities) this->removeEntity(e);
			to_remove_entities.clear();
		}

	private: // Methods
		/*
		* Removes the component of type Component from entity e.
		* This also updates the bitfield of the entity.
		*/
		template<class Component>
		void removeComponent(uint32_t e)
		{
			constexpr size_t index = type_index<Component, Components...>::value;
			std::get<index>(components).erase(e);
			entities.at(e).removeComponent<Component>();
		}

		/*
		* Removes the components of types Components... from entity e.
		* This also updates the bitfield of the entity.
		*/
		template<class... Components>
		void removeComponents(uint32_t e)
		{
			(removeComponent<Components>(e), ...);
		}

		/*
		* Updates the filter of type Filter for the given entity. This checks if the entity e has all components
		* of the filter, and if so adds the entity to the filter.
		*/
		template<class Filter>
		void updateFilter(uint32_t e)
		{
			bool res = filter_helper<Filter>::update(entities, e);

			// Index of filter in the list of filter types
			constexpr size_t index = type_index<Filter, Filters...>::value;

			auto& filter = std::get<index>(filters);

			if (res)
			{
				if (!filter.hasEntity(e))
					filter.addEntity(e);
			}
			else
			{
				if (filter.hasEntity(e))
					filter.removeEntity(e);
			}
		}

		template<class... Filters>
		void updateFilters(uint32_t e)
		{
			(updateFilter<Filters>(e), ...);
		}
	};
}