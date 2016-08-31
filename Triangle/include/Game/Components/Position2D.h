#pragma once
#include "Modules\ECS\ECS.h"
#include <glm\glm.hpp>

namespace Components
{
	class Position2D : public Component
	{
	public:
		Position2D() {}
		Position2D(float x, float y) : pos(x, y) {}
		Position2D(glm::vec2 v) : pos(v) {}

		glm::vec2 pos;
	};
}
