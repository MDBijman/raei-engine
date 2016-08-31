#pragma once
#include "Modules\ECS\ECS.h"
#include <glm\glm.hpp>

namespace Components
{
	class Velocity3D : public Component
	{
	public:
		Velocity3D() {}
		Velocity3D(float x, float y, float z) : vel(x, y, z) {}
		Velocity3D(glm::vec3 v) : vel(v) {}

		glm::vec3 vel;
	};
}
