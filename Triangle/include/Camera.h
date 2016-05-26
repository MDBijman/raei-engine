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
		projectionMatrix = glm::perspective(glm::radians(60.0f), dimensions.x / dimensions.y, near, far);
		viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, zoom));
	}


	float zoom, fov, near, far;
	glm::vec2 dimensions;
	glm::mat4 projectionMatrix, viewMatrix;
};


#define near
#define far