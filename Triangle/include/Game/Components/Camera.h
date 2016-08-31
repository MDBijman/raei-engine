#pragma once
#include "Modules\ECS\Component.h"
#include <glm\glm.hpp>

namespace Components
{
	class Camera : public Component
	{
	public:
		// The dimensions of this camera.
		glm::vec2 dimensions;

		// Field of view, near plane, and far plane of this camera 
		float fov, nearPlane, farPlane;
	};
}