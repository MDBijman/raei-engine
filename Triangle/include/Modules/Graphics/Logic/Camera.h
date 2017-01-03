#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Graphics
{
	class Camera
	{
	public:
		Camera(float left, float right, float bottom, float top, float clipNear, float clipFar)
		{
			matrices.projection = glm::ortho<float>(left, right, bottom, top, clipNear, clipFar);
			
			moveTo(glm::vec3(), glm::vec3());
		}

		Camera(float fov, float aspect, float clipNear, float clipFar)
		{
			matrices.projection = glm::perspective(glm::radians(fov), aspect, clipNear, clipFar);
			moveTo(glm::vec3(), glm::vec3());
		}

		const auto& getMatrices()
		{
			return matrices;
		}

		void moveTo(glm::vec3 position, glm::vec3 rotation)
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);

			glm::mat4 rotationMatrix;
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), { 1.0, 0.0, 0.0 });
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), { 0.0, 1.0, 0.0 });
			rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), { 0.0, 0.0, 1.0 });

			matrices.view = glm::inverse(translationMatrix * rotationMatrix);
		}

	protected:
		struct
		{
			glm::mat4 view;
			glm::mat4 projection;
		} matrices;
	};
}