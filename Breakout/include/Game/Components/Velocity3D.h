#pragma once
#include "Modules/ECS/Component.h"
#include <glm/vec3.hpp>

namespace Components
{
	class Velocity3D : public ecs::Component
	{
	public:
		Velocity3D() {}
		Velocity3D(float x, float y, float z) : vel(x, y, z) {}
		explicit Velocity3D(glm::vec3 v) : vel(v) {}

		glm::vec3 vel;
	};
}
