#pragma once

#include <glm\glm.hpp>
#include <string>

namespace Importers
{
	namespace Parser {
		glm::vec3 vec3f(std::string line, int& offset);
		glm::vec2 vec2f(std::string line, int& offset);
		int parseInt(std::string line, int& offset);
		float parseFloat(std::string line, int& offset);
	}
}
