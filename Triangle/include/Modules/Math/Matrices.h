#pragma once

#include <glm/gtc/matrix_transform.hpp>

#undef near
#undef far 

namespace Math
{

	glm::mat4 calculateProjection(float fov, float near, float far, glm::vec2 dimensions);

	glm::mat4 calculateView(glm::vec3 front, glm::vec3 up, glm::vec3 pos);
}

#define near
#define far