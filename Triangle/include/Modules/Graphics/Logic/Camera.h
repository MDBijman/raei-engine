#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Graphics
{
	class Camera
	{
	public:
		Camera(glm::vec3 p, glm::vec3 r, glm::mat4 proj) : position(p), rotation(r)
		{
			matrices.projection = proj;
			updateView();
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

	protected:
		struct
		{
			glm::mat4 view;
			glm::mat4 projection;
		} matrices;

		glm::vec3 position;
		glm::vec3 rotation;

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
	};

#undef near
#undef far
	class OrthoCamera : public Camera
	{
	public:
		OrthoCamera(glm::vec3 position, glm::vec3 rotation, float left, float right, float bottom, float top, float near, float far) :
			Camera(
				position, rotation,
				glm::ortho(left, right, bottom, top, near, far)
			) {}

	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(glm::vec3 position, glm::vec3 rotation, float fov, float aspect, float near, float far) : 
			Camera(
				position, rotation,
				glm::perspective(glm::radians(fov), aspect, near, far)
			) {}
	};
#define near
#define far

}