#pragma once
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
