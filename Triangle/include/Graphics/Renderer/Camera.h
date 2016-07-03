#pragma once

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera
{
public:
	Camera(const glm::vec2 dimensions, const float fov, const float nearPlane, const float farPlane) : FOV(fov), NEAR_PLANE(nearPlane), FAR_PLANE(farPlane), DIMENSIONS(dimensions)
	{
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront.y = sin(glm::radians(pitch));
		cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(cameraFront);
	}

	void forward(const float velocity, const float dt)
	{
		cameraPos += cameraFront * (velocity * dt);
	}
	
	void backward(const float velocity, const float dt)
	{
		cameraPos -= cameraFront * (velocity * dt);
	}
	
	void left(const float velocity, const float dt)
	{
		cameraPos -= glm::cross(cameraFront, cameraUp) * velocity * dt;
	}

	void right(const float velocity, const float dt)
	{
		cameraPos += glm::cross(cameraFront, cameraUp) * velocity * dt;
	}

	void rotate(const glm::vec2 r, const float dt)
	{
		rotate(r.x, r.y, dt);
	}

	void rotate(const float yaw, const float pitch, const float dt)
	{
		this->yaw += yaw * dt;
		this->pitch += pitch * dt;
		cameraFront.x = cos(this->yaw) * cos(this->pitch);
		cameraFront.y = sin(this->pitch);
		cameraFront.z = sin(this->yaw) * cos(this->pitch);
		cameraFront = glm::normalize(cameraFront);
	}

	const glm::vec3& getPosition() const
	{
		return cameraPos;
	}

	const glm::vec3& getDirection() const
	{
		return cameraFront;
	}

	const glm::vec3& getUp() const
	{
		return cameraUp;
	}

	const float& getPitch() const
	{
		return pitch;
	}

	const float& getYaw() const
	{
		return yaw;
	}

	const glm::vec2 DIMENSIONS;
	const float FOV, NEAR_PLANE, FAR_PLANE;

private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float pitch;
	float yaw;
};
