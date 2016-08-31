#pragma once
#include "Modules\ECS\ECS.h"

namespace Components
{
	class Position3D : public Component
	{
	public:
		Position3D() {}
		Position3D(float x, float y, float z) : pos(x, y, z) {}
		Position3D(glm::vec3 v) : pos(v) {}

		glm::vec3 pos;
	};
}
