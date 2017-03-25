#pragma once
#include "Modules/ECSFactory/Traits.h"
#include "Modules/ECS/Component.h"
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

namespace Components
{
	class Dummy : public ECS::Component
	{
	public:
		Dummy(int x) : x(x + 2){}
		int x;
	};
}

template<> struct ECS::FromLua<Components::Dummy>
{
	static Components::Dummy construct(const std::vector<ECS::TypeVariant>& args)
	{
		std::cout << "Creating Dummys" << std::endl;
		return Components::Dummy(3);
	}
};