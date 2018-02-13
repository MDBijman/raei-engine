#pragma once
#include "Modules/ECS/Component.h"
#include <glm/vec3.hpp>

namespace Components
{
	class Position3D : public ecs::Component
	{
	public:
		Position3D() {}
		Position3D(float x, float y, float z) : pos(x, y, z) {}
		explicit Position3D(glm::vec3 v) : pos(v) {}

		glm::vec3 pos;
	};
}
