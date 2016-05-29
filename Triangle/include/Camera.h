#pragma once
// Windows defines
#undef near
#undef far

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera
{
public:
	Camera(const glm::vec2 dimensions, const float zoom, const float fov, const float near, const float far) : zoom(zoom), fov(fov), near(near), far(far), dimensions(dimensions)
	{
		projectionMatrix = glm::perspective(glm::radians(fov), dimensions.x / dimensions.y, near, far);

		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		viewMatrix = glm::lookAt(
			cameraPos,
			cameraPos + cameraFront,
			cameraUp
		);
	}

	void update()
	{
		cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront.y = sin(glm::radians(pitch));
		cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(cameraFront);

		viewMatrix = glm::lookAt(
			cameraPos,
			cameraPos+ cameraFront,
			cameraUp
		);
	}

	void forward(float velocity)
	{
		cameraPos += cameraFront * velocity;
	}
	
	void backward(float velocity)
	{
		cameraPos -= cameraFront * velocity;
	}
	
	void left(float velocity)
	{
		cameraPos -= glm::cross(cameraFront, cameraUp) * velocity;
	}

	void right(float velocity)
	{
		cameraPos += glm::cross(cameraFront, cameraUp) * velocity;
	}

	void rotate(glm::vec2 r)
	{
		pitch += r.y;
		yaw += r.x;
	}

	void rotate(float yaw, float pitch)
	{
		this->yaw += yaw;
		this->pitch += pitch;
	}

	glm::mat4 getProjection()
	{
		return projectionMatrix;
	}

	glm::mat4 getView()
	{
		return viewMatrix;
	}

	glm::vec2 dimensions;

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float pitch, yaw;

	float zoom, fov, near, far;
	glm::mat4 projectionMatrix, viewMatrix;
};


#define near
#define far