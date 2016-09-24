#include "Modules/Math/Matrices.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#undef near
#undef far

namespace Math
{
	glm::mat4 calculateProjection(float fov, float near, float far, glm::vec2 dimensions)
	{
		return glm::perspective(
			glm::radians(fov),
			dimensions.x / dimensions.y,
			near,
			far
		);
	}

	glm::mat4 calculateView(glm::vec3 front, glm::vec3 up, glm::vec3 pos)
	{
		return glm::lookAt(
			pos,
			pos + front,
			up
		);
	}
}

#define near
#define far
