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

		struct group_thread
		{
			group_thread(std::shared_ptr<std::atomic_bool> keep_running, std::shared_ptr<std::atomic_int> workers_running,
				std::shared_ptr<std::condition_variable> parent, std::shared_ptr<std::mutex> parent_mutex,
				std::shared_ptr<memory::safe_queue<10, std::pair<system*, Time::Timer2*>>> queue, ecs& parent_ecs) : 
				parent_wakeup(parent), parent_ecs(parent_ecs), keep_running(keep_running), parent_wakeup_mutex(parent_mutex),
				workers_running(workers_running), work_queue(queue), thread(&group_thread::run, this) {}

			std::shared_ptr<std::atomic_bool> keep_running;
			std::shared_ptr<std::condition_variable> parent_wakeup;
			std::shared_ptr<std::mutex> parent_wakeup_mutex;
			std::shared_ptr<std::atomic_int> workers_running;
			std::condition_variable wakeup;
			std::mutex wakeup_mutex;
			bool should_wake = false;
			ecs& parent_ecs;

			std::shared_ptr<memory::safe_queue<10, std::pair<system*, Time::Timer2*>>> work_queue;
			std::thread thread;

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

						auto pair = work.value();
						pair.first->update(parent_ecs, pair.second->dt());
						pair.second->zero();
					}

					{
						std::scoped_lock<std::mutex> sl(*parent_wakeup_mutex);
						(*workers_running)--;
						parent_wakeup->notify_all();
					}
				}
			}
		};

		struct group
		{
			using group_work_queue = memory::safe_queue<10, std::pair<system*, Time::Timer2*>>;

			group(ecs& parent_ecs) : keep_running(std::make_shared<std::atomic_bool>(true)),
				workers_running(std::make_shared<std::atomic_int>()), wakeup(std::make_shared<std::condition_variable>()),
				parent_ecs(parent_ecs), work_queue(std::make_shared<group_work_queue>()),
				wakeup_mutex(std::make_shared<std::mutex>())
			{
				for (int i = 0; i < 4; i++)
					threads.at(i) = std::make_unique<group_thread>(keep_running, workers_running, wakeup, wakeup_mutex
						work_queue, parent_ecs);
			}

			group(group&& o) : parent_ecs(o.parent_ecs),
				systems(std::move(o.systems)), workers_running(std::move(o.workers_running)),
				keep_running(std::move(o.keep_running)), work_queue(std::move(o.work_queue)),
				system_timers(std::move(o.system_timers)), threads(std::move(o.threads)), wakeup(std::move(o.wakeup)),
				wakeup_mutex(std::move(o.wakeup_mutex)) {}

			void update()
			{
				if (systems.size() == 0) return;
				assert(systems.size() == system_timers.size());

				// Put work in threads
				for (std::size_t i = 0; i < systems.size(); i++)
					work_queue->enqueue({ systems.at(i).get(), &system_timers.at(i) });

				workers_running->store(4);

				// Notify threads to run systems
				for (auto& t : threads)
				{
					t->should_wake = true;
					t->wakeup.notify_all();
				}

				std::unique_lock<std::mutex> ul(*wakeup_mutex);
				wakeup->wait(ul, [this]{ 
					return this->workers_running->load() == 0; 
				});
			}

			void add_system(std::unique_ptr<system> s)
			{
				systems.push_back(std::move(s));
				system_timers.push_back(Time::Timer2());
			}

		private:
			std::array<std::unique_ptr<group_thread>, 4> threads;

			std::shared_ptr<std::atomic_bool> keep_running;

			std::shared_ptr<std::condition_variable> wakeup;
			std::shared_ptr<std::mutex> wakeup_mutex;

			std::shared_ptr<std::atomic_int> workers_running;
			std::shared_ptr<group_work_queue> work_queue;
			ecs& parent_ecs;

			std::vector<std::unique_ptr<system>> systems;
			std::vector<Time::Timer2> system_timers;
		};

	public:
		using group_id = uint32_t;

		base_system_manager(ecs& parent_ecs) : parent_ecs(parent_ecs) {}

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

	private:
		ecs & parent_ecs;

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
	private:
		using component_tuple = std::tuple<Components...>;
		using filter_tuple = std::tuple<Filters...>;

		using system = System<component_tuple, filter_tuple>;
		using system_manager = base_system_manager<base_manager<component_tuple, filter_tuple>>;

		static constexpr int ComponentCount = sizeof...(Components);

		template<class... Components>
		struct filter_lock {};

		template<class... Components>
		struct filter_lock<filter<Components...>>
		{
			filter_lock(base_manager<component_tuple, filter_tuple>& ecs) : ecs(ecs)
			{
				std::lock_guard<std::mutex> l(ecs.master_component_mutex);
				FilterHelper<ComponentCount, filter<Components...>>::lock(ecs.component_mutexes);
			};
			~filter_lock()
			{
				if (!moved)
					FilterHelper<ComponentCount, filter<Components...>>::unlock(ecs.component_mutexes);
			};
			filter_lock(const filter_lock&) = delete;
			filter_lock(filter_lock&& o) : ecs(o.ecs)
			{
				o.moved = true;
				moved = false;
			}
			void operator=(const filter_lock&) = delete;
			void operator=(filter_lock&& o)
			{
				o.moved = true;
				moved = false;
			};

		private:
			bool moved = false;
			base_manager<component_tuple, filter_tuple>& ecs;
		};

		template<class... Components>
		friend struct filter_lock;

		/*
		* Checks filters for the given entity to see if it matches, if so, update.
		* Helper struct for checkFilters().
		*/
		template<int ComponentCount, class Filter>
		struct FilterHelper {};

		template<int ComponentCount, class... ComponentTypes>
		struct FilterHelper<ComponentCount, filter<ComponentTypes...>>
		{
			template<class T>
			static bool update(T& entities, uint32_t e)
			{
				return entities.at(e).template hasComponents<ComponentTypes...>();
			}

			static void lock(std::array<std::mutex, ComponentCount>& locks)
			{
				(locks.at(type_index<ComponentTypes, Components...>::value).lock(), ...);
			}

			static void unlock(std::array<std::mutex, ComponentCount>& locks)
			{
				(locks.at(type_index<ComponentTypes, Components...>::value).unlock(), ...);
			}
		};

	public:
		base_manager() : child_system_manager(*this) { }

		/* Entity Methods */

		uint32_t createEntity()
		{
			Entity<Components...> e = Entity<Components...>(++entityCount);
			entities.insert(std::make_pair(e.id, e));
			return entityCount;
		}

		void removeEntity(uint32_t e)
		{
			removeComponents<Components...>(e);
			updateFilters<Filters...>(e);
			entities.erase(e);
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
		std::enable_if_t<std::is_base_of_v<Component, ComponentType>, ComponentType&> addComponent(uint32_t entityId, ComponentType&& c)
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
		std::pair<filter_lock<Filter>, const std::unordered_set<uint32_t>&> filterEntities()
		{
			// We can only create components that derive from Component class
			constexpr size_t index = type_index<Filter, Filters...>::value;

			return std::pair<filter_lock<Filter>, const std::unordered_set<uint32_t>&>(
				filter_lock<Filter>(*this), std::get<index>(filters).entities);
		}

		system_manager& get_system_manager()
		{
			return this->child_system_manager;
		}

		void update()
		{
			this->child_system_manager.update();
		}

	private:
		system_manager child_system_manager;

		template<class Component>
		void removeComponent(uint32_t e)
		{
			constexpr size_t index = type_index<Component, Components...>::value;
			std::get<index>(components).erase(e);
			entities.at(e).removeComponent<Component>();
		}

		template<class... Components>
		void removeComponents(uint32_t e)
		{
			(removeComponent<Components>(e), ...);
		}

		template<class filter>
		void updateFilter(uint32_t e)
		{
			bool res = FilterHelper<ComponentCount, filter>::update(entities, e);

			// Index of filter in the list of filter types
			constexpr size_t index = type_index<filter, Filters...>::value;

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

		/*
		* Containers
		*/

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
	};
}