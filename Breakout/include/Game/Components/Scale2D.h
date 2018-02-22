#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <variant>

namespace Components
{
	class Scale2D : public ecs::Component
	{
	public:
		Scale2D() = default;
		Scale2D(double x, double y) : scale(x, y) {}
		Scale2D(float x, float y) : scale(x, y) {}
		Scale2D(glm::vec2 a) : scale(a) {}
		glm::vec2 scale;
	};
}
