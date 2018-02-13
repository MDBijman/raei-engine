#pragma once
#include "Modules/TemplateUtils/TypeIndex.h"

#include <stdint.h>
#include <bitset>

namespace ecs
{
	template<class... ComponentTypes>
	class Entity
	{
	public:
		Entity() = delete;

		explicit Entity(uint32_t id) : id(id) {}

		uint32_t id;

		/*
			Returns true if this entity has a component of each type.
			Base case
		*/
		template<class ComponentType>
		bool hasComponents()
		{
			return hasComponent<ComponentType>();
		}

		/*
			Returns true if this entity has a component of each type.
			Recursive case
		*/
		template<class ComponentType, class SecondType, class... OtherComponentTypes>
		bool hasComponents()
		{
			if (!hasComponent<ComponentType>())
				return false;
			return hasComponents<SecondType, OtherComponentTypes...>();
		}

		/*
			Returns true if this entity has a component of the given type.
		*/
		template<class ComponentType>
		bool hasComponent()
		{
			constexpr int index = type_index<ComponentType, ComponentTypes...>::value;
			return components[index];
		}

		/*
			Sets the bit of the corresponding type to 1.
		*/
		template<class ComponentType>
		void addComponent()
		{
			constexpr int index = type_index<ComponentType, ComponentTypes...>::value;
			components.set(index);
		}

		/*
			Sets the bit of the corresponding type to 0.
		*/
		template<class ComponentType>
		void removeComponent()
		{
			constexpr int index = type_index<ComponentType, ComponentTypes...>::value;
			components.set(index, false);
		}

		/*
			A bit for each type. 1 implies that this entity has the component.
		*/
		std::bitset<sizeof...(ComponentTypes)> components;
	};
}
