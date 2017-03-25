#pragma once
#include "Modules/ECS/Component.h"
#include <glm/vec2.hpp>

namespace Components
{
	class Velocity2D : public ECS::Component
	{
	public:
		Velocity2D() {}
		Velocity2D(float x, float y) : vel(x, y) {}
		explicit Velocity2D(glm::vec2 v) : vel(v) {}

		glm::vec2 vel;
	};
}
