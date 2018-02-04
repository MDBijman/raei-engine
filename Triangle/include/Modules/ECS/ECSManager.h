#pragma once
#include "Component.h"
#include "Entity.h"
#include "System.h"
#include "Modules/TemplateUtils/TypeIndex.h"

#include <unordered_map>
#include <unordered_set>
#include <stdint.h>
#include <algorithm>

namespace ECS
{
	/*
		Template parameter classes
	*/

	// Holds a list of all component types
	template<class...> struct ComponentList {};

	// Holds a list of component types
	template<class...> struct Filter
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

	// Give a pack of filter types
	template<class...> struct FilterList {};

	/*
		ECSManager
	*/

	template<typename ATuple, typename BTuple>
	class ECSManager;

	template<class... A, class... B>
	class ECSManager<ComponentList<A...>, FilterList<B...>>
	{
	public:
		ECSManager() {}

		~ECSManager()
		{
			for (auto system : systems)
				delete system;
		}

		/*
			Entity Methods
		*/

		uint32_t createEntity()
		{
			Entity<A...> e = Entity<A...>(++entityCount);
			entities.insert(std::make_pair(e.id, e));
			return entityCount;
		}

		void removeEntity(uint32_t e)
		{
			removeComponents<A...>(e);
			updateFilters<B...>(e);
			entities.erase(e);
		}

		/*
			Component Methods
		*/

		/*
			Returns ComponentType of the given entity.
		*/
		template<class ComponentType>
		ComponentType& getComponent(uint32_t e)
		{
			constexpr auto index = type_index<ComponentType, A...>::value;
			return std::get<index>(components).at(e);
		}

		/*
			Sets the parent of the given component and adds it to the vector of components of that type.
		*/
		template<class ComponentType>
		std::enable_if_t<std::is_base_of_v<Component, ComponentType>, ComponentType&> addComponent(uint32_t entityId, ComponentType&& c)
		{
			// Set the parent of the component
			c.parent = entityId;

			// Index of ComponentType in the list of component types
			constexpr size_t index = type_index<ComponentType, A...>::value;

			// Add the component to the vector of components of that type
			std::get<index>(components).insert(std::make_pair(entityId, std::forward<ComponentType>(c)));

			// Add the component type to the bitfield of the entity
			entities.at(entityId).template addComponent<ComponentType>();

			updateFilters<B...>(entityId);

			return std::get<index>(components).find(entityId)->second;
		}

		/*
			System Methods
		*/

		/*
			Returns all entities that match the signature.
		*/
		template<class Filter>
		std::unordered_set<uint32_t> filterEntities()
		{
			// We can only create components that derive from Component class
			constexpr int index = type_index<Filter, B...>::value;

			return std::get<index>(filters).entities;
		}

		/*
			Adds the given system to this manager.
		*/
		void addSystem(System<ComponentList<A...>, FilterList<B...>>* s)
		{
			systems.push_back(s);
		}

		/*
			Adds a system of the given type to this manage.
		*/
		template<class SystemType, typename... Args>
		std::enable_if_t<std::is_base_of_v<System<ComponentList<A...>, FilterList<B...>>, SystemType>, void> addSystem(Args... args)
		{
			systems.push_back(new SystemType(args...));
		}

		/*
			Updates every system of this manager.
		*/
		void updateSystems(double dt)
		{
			for (auto& s : systems)
				s->update(*this, dt);
		}

	private:
		template<class Component>
		void removeComponents(uint32_t e)
		{
			constexpr size_t index = type_index<Component, A...>::value;
			std::get<index>(components).erase(e);
			entities.at(e).removeComponent<Component>();
		}

		template<class Component, class SecondComponent, class... Components>
		void removeComponents(uint32_t e)
		{
			removeComponents<Component>(e);
			removeComponents<SecondComponent, Components...>(e);
		}

		/*
			Checks filters for the given entity to see if it matches, if so, update.
			Helper struct for checkFilters().
		*/
		template<class... ComponentTypes>
		struct UpdateFilter {};

		template<class... ComponentTypes>
		struct UpdateFilter<Filter<ComponentTypes...>>
		{
			template<class T>
			static bool call(T& entities, uint32_t e)
			{
				return entities.at(e).template hasComponents<ComponentTypes...>();
			}
		};

		/*
			Base Case
		*/
		template<class Filter>
		void updateFilters(uint32_t e)
		{
			bool res = UpdateFilter<Filter>::call(entities, e);

			// Index of filter in the list of filter types
			constexpr size_t index = type_index<Filter, B...>::value;

			auto& filter = std::get<index>(filters);

			if (res)
			{
				if (!filter.hasEntity(e))
					filter.addEntity(e);
			}
			else
			{
				if(filter.hasEntity(e))
					filter.removeEntity(e);
			}
		}

		/*
			Recursive case
		*/
		template<class FirstFilter, class SecondFilter, class... OtherFilters>
		void updateFilters(uint32_t e)
		{
			updateFilters<FirstFilter>(e);
			updateFilters<SecondFilter, OtherFilters...>(e);
		}

		/*
			Containers
		*/

		// Tuple for each filter type
		std::tuple<B...> filters;

		// Tuple of maps for each component type
		std::tuple<std::unordered_map<uint32_t, A>...> components;

		// Map of entity ids to entities
		std::unordered_map<uint32_t, Entity<A...>> entities;

		// Vector of systems
		std::vector<System<ComponentList<A...>, FilterList<B...>>*> systems;

		uint32_t entityCount = 0;
	};
}