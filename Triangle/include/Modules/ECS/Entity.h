#pragma once
#include "Modules\TemplateUtils\TypeIndex.h"

#include <stdint.h>
#include <bitset>

template<class... ComponentTypes>
class Entity
{
private:
	template<class A, class B>
	friend class ECSManager;

	Entity() = delete;

	Entity(uint32_t id) : id(id) {}

	uint32_t id;

	/*
		Returns true if this entity has a component of each type.
	*/

	/*
		Base case
	*/
	template<class ComponentType>
	bool hasComponents()
	{
		return hasComponent<ComponentType>();
	}

	/*
		Recursive case
	*/
	template<class ComponentType, class SecondType, class... ComponentTypes>
	bool hasComponents()
	{
		if (!hasComponent<ComponentType>())
			return false;
		return hasComponents<SecondType, ComponentTypes...>();
	}

	/*
		Returns true if this entity has the given component type.
	*/
	template<class ComponentType>
	bool hasComponent()
	{
		constexpr int index = type_index<ComponentType, ComponentTypes...>::value;
		return components[index];
	}

	/*
		Sets the corresponding bit of the type to 1.
	*/
	template<class ComponentType>
	void addComponent()
	{
		constexpr int index = type_index<ComponentType, ComponentTypes...>::value;
		components.set(index);
	}

	/*
		Sets the corresponding bit of the type to 0.
	*/
	template<class ComponentType>
	void removeComponent(size_t index)
	{
		constexpr int index = type_index<ComponentType, ComponentTypes...>::value;
		components.set(index, false);
	}

	/*
		A bit for each type. 1 implies that this entity has the component. 
	*/
	std::bitset<sizeof...(ComponentTypes)> components;
};
