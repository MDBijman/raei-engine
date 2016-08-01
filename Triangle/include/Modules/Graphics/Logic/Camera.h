#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

class Camera
{
public:
	Camera() : Camera(glm::vec2(0, 0), 0.0f, 0.0f, 0.0f)
	{}

	Camera(const glm::vec2 dimensions, const float fov, const float nearPlane, const float farPlane) : FOV(fov), NEAR_PLANE(nearPlane), FAR_PLANE(farPlane), DIMENSIONS(dimensions), yaw(0.0f), pitch(0.0f)
	{
		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront.y = sin(glm::radians(pitch));
		cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(cameraFront);
	}

	// Moves this camera forward.
	void forward(const float velocity, const float dt)
	{
		cameraPos += cameraFront * (velocity * dt);
	}
	
	// Moves this camera backward.
	void backward(const float velocity, const float dt)
	{
		cameraPos -= cameraFront * (velocity * dt);
	}

	// Moves this camera to the left.
	void left(const float velocity, const float dt)
	{
		cameraPos -= glm::cross(cameraFront, cameraUp) * velocity * dt;
	}

	// Moves the camera to the right.
	void right(const float velocity, const float dt)
	{
		cameraPos += glm::cross(cameraFront, cameraUp) * velocity * dt;
	}

	// Rotates the camera.
	void rotate(const glm::vec2 r, const float dt)
	{
		rotate(r.x, r.y, dt);
	}

	// Rotates the camera.
	void rotate(const float yaw, const float pitch, const float dt)
	{
		this->yaw += yaw * dt;
		this->pitch += pitch * dt;
		cameraFront.x = cos(this->yaw) * cos(this->pitch);
		cameraFront.y = sin(this->pitch);
		cameraFront.z = sin(this->yaw) * cos(this->pitch);
		cameraFront = glm::normalize(cameraFront);
	}

	// Returns the position of this camera.
	const glm::vec3& getPosition() const
	{
		return cameraPos;
	}

	// Returns the direction of this camera.
	const glm::vec3& getDirection() const
	{
		return cameraFront;
	}

	// Returns the up vector of this camera.
	const glm::vec3& getUp() const
	{
		return cameraUp;
	}

	// Returns the pitch of this camera.
	float getPitch() const
	{
		return pitch;
	}

	// Returns the yaw of this camera.
	float getYaw() const
	{
		return yaw;
	}

	// Returns a reference to the dimension of this camera.
	const glm::vec2& getDimensions() const
	{
		return DIMENSIONS;
	}

	// Returns the field of view of this camera.
	float getFOV() const
	{
		return FOV;
	}

	// Returns the near plane of this camera.
	float getNearPlane() const
	{
		return NEAR_PLANE;
	}

	// Returns the far plane of this camera.
	float getFarPlane() const
	{
		return FAR_PLANE;
	}

private:
	// Position of this camera.
	glm::vec3 cameraPos;

	// Direction front of this camera.
	glm::vec3 cameraFront;

	// Direction upwards of this camera.
	glm::vec3 cameraUp;

	// Pitch of this camera
	float pitch;

	// Yaw of this camera.
	float yaw;


	// The dimensions of this camera.
	glm::vec2 DIMENSIONS;

	// Field of view, near plane, and far plane of this camera 
	float FOV, NEAR_PLANE, FAR_PLANE;
};
