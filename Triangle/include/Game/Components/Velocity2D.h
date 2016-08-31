#pragma once
#include "Modules\ECS\ECS.h"
#include <glm\glm.hpp>

namespace Components
{
	class Velocity2D : public Component
	{
	public:
		Velocity2D() {}
		Velocity2D(float x, float y) : vel(x, y) {}
		Velocity2D(glm::vec2 v) : vel(v) {}

		glm::vec2 vel;
	};
}
