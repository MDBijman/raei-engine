#pragma once
#include "Modules/ECSFactory/Traits.h"
#include "Modules/ECS/Component.h"
#include <glm/vec2.hpp>
#include <string>
#include <variant>
#include <vector>

namespace Components
{
	class Position2D : public ecs::Component
	{
	public:
		Position2D() {}
		Position2D(double x, double y) : pos(x, y) {}
		Position2D(float x, float y) : pos(x, y) {}
		explicit Position2D(glm::vec2 v) : pos(v) {}
		glm::vec2 pos;
	};
}

template<> struct ecs::FromLua<Components::Position2D>
{
	static Components::Position2D construct(const std::vector<ecs::TypeVariant>& args)
	{
		if(args.size() != 2) throw std::invalid_argument("Too many parameters");

		auto x = std::get_if<double>(&args[0]);
		if (!x) throw std::invalid_argument("Wrong type for x");

		auto y = std::get_if<double>(&args[1]);
		if (!y) throw std::invalid_argument("Wrong type for y");

		return Components::Position2D(glm::vec2(*x, *y));
	}
};