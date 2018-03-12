#pragma once
#include "Component.h"
#include "Entity.h"
#include "System.h"
#include "SystemManager.h"
#include "Modules/TemplateUtils/TypeIndex.h"
#include "Modules/TemplateUtils/TemplatePackSize.h"
#include "Modules/Time/Timer.h"

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

		using system = System<base_manager<component_tuple, filter_tuple>>;
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

		void operator=(base_manager&& o)
		{
			this->child_system_manager = std::move(o.child_system_manager);
			this->to_remove_entities = std::move(o.to_remove_entities);
			this->filters = std::move(o.filters);
			this->components = std::move(o.components);
			this->entities = std::move(o.entities);
			this->entityCount = o.entityCount;
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