#pragma once
#include "Modules/ECSFactory/Traits.h"
#include <glm/vec2.hpp>
#include <string>
#include <variant>

namespace Components
{
	class Scale2D : public ecs::Component
	{
	public:
		Scale2D() = default;
		Scale2D(glm::vec2 a) : scale(a) {}
		glm::vec2 scale;
	};
}

template<> struct ecs::FromLua<Components::Scale2D>
{
	// TODO move type checking and such inside interface?
	static Components::Scale2D construct(const std::vector<ecs::TypeVariant>& args)
	{
		if(args.size() != 2) throw std::invalid_argument("Too many parameters");

		auto x = std::get_if<double>(&args[0]);
		if (!x) throw std::invalid_argument("Wrong type for x");

		auto y = std::get_if<double>(&args[1]);
		if (!y) throw std::invalid_argument("Wrong type for y");

		return Components::Scale2D(glm::vec2(*x, *y));
	}
};