#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#undef near
#undef far

namespace Graphics
{
	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(float fov, float aspect, float near, float far)
		{
			position = glm::vec3(0.0f, 0.0f, 1.0f);
			rotation = glm::vec3(0.0f, 0.0f, 0.0f);

			matrices.projection = glm::perspective(glm::radians(fov), aspect, near, far);
			updateView();
			//(-1.0f, 1.0f, -1.0f, 1.0f, -1.f, 100.0f);
		}

		// Moves the camera to the right.
		void move(const glm::vec3 movement, const float dt)
		{
			position += movement * dt;
			updateView();
		}

		// Rotates the camera.
		void rotate(const glm::vec3 r, const float dt)
		{
			rotation += r * dt;
			updateView();
		}

		const auto& getMatrices()
		{
			return matrices;
		}

	private:
		void updateView()
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);

			glm::mat4 rotationMatrix;
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), { 1.0, 0.0, 0.0 });
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), { 0.0, 1.0, 0.0 });
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), { 0.0, 0.0, 1.0 });

			matrices.view = glm::inverse(translationMatrix * rotationMatrix);
		}

		struct
		{
			glm::mat4 view;
			glm::mat4 projection;
		} matrices;

		glm::vec3 position;
		glm::vec3 rotation;
	};
}

#define near
#define far