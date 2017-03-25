#pragma once
#include "Modules/ECS/Component.h"
#include <glm/vec3.hpp>
#include <glm/detail/func_geometric.hpp>

namespace Components
{
	class Orientation3D : public ECS::Component
	{
	public:
		Orientation3D() : front(1, 0, 0), up(0, 1, 0), pitch(0), yaw(0) {}

		// Rotates the object.
		void rotate(const float yaw, const float pitch, const float dt)
		{
			this->yaw += yaw * dt;
			this->pitch += pitch * dt;
			front.x = cos(this->yaw) * cos(this->pitch);
			front.y = sin(this->pitch);
			front.z = sin(this->yaw) * cos(this->pitch);
			front = glm::normalize(front);
		}

		// Direction front.
		glm::vec3 front;

		// Direction upwards.
		glm::vec3 up;

		// Pitch.
		float pitch;

		// Yaw.
		float yaw;


	};
}
