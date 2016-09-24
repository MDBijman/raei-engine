#pragma once
#include <glm/glm.hpp>

namespace Graphics
{
	namespace Data
	{
		class UniformMVP
		{
		public:
			glm::mat4 projectionMatrix;
			glm::mat4 modelMatrix;
			glm::mat4 viewMatrix;
		};
	}
}
