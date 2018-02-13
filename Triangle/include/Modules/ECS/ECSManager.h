#pragma once
#include "Component.h"
#include "Entity.h"
#include "System.h"
#include "Modules/TemplateUtils/TypeIndex.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"
#include "Modules/Time/Timer.h"


#include <unordered_map>
#include <unordered_set>
#include <stdint.h>
#include <algorithm>
#include <mutex>
#include <atomic>

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

		struct group
		{
			group() {}
			group(group&& o) : systems(std::move(o.systems)) {}

			void update(ecs& parent_ecs)
			{
				std::vector<std::thread> threads;
				for (auto& pair : systems)
				{
					threads.push_back(std::thread([&pair, &parent_ecs]() {
						pair.first->update(parent_ecs, pair.second.dt());
						pair.second.zero();
					}));
				}

				for (auto& thread : threads)
				{
					thread.join();
				}
			}

			std::vector<std::pair<std::unique_ptr<system>, Time::Timer2>> systems;
		};

	public:
		using group_id = uint32_t;

		base_system_manager(ecs& parent_ecs) : parent_ecs(parent_ecs) {}

		group_id create_group()
		{
			counter++;
			group g = group();
			system_groups.insert({ counter, std::move(g) });
			order.push_back(counter);
			return counter;
		}

		group& get_group(group_id id)
		{
			return system_groups.find(id);
		}

		void add_to_group(group_id id, std::unique_ptr<system> sys)
		{
			system_groups.at(id).systems.push_back({ std::move(sys), Time::Timer2() });
		}

		void update(ecs& parent_ecs)
		{
			for (auto id : order)
			{
				system_groups[id].update(parent_ecs);
			}
		}

		void stop_group(group_id id)
		{
			system_groups.at(id).should_stop.store(true);
			system_groups.at(id).thread.join();
		}

	private:
		ecs& parent_ecs;

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
			this->child_system_manager.update(*this);
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