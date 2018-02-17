#pragma once
#include "Modules/ECSFactory/Traits.h"
#include "Modules/ECS/Component.h"
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

namespace Components
{
	class Dummy : public ecs::Component
	{
	public:
		Dummy(int x) : x(x + 2){}
		int x;
	};
}

template<> struct ecs::FromLua<Components::Dummy>
{
	static Components::Dummy construct(const std::vector<ecs::TypeVariant>& args)
	{
		std::cout << "Creating Dummys" << std::endl;
		return Components::Dummy(3);
	}
};