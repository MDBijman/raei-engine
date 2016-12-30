#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>

namespace Importers
{
	class Obj
	{
	public:
		using Vertex = std::tuple<glm::vec3, glm::vec2, glm::vec3>;
		using Index = uint32_t;

		std::vector<Vertex> vertices;
		std::vector<Index> indices;
		

		static Obj* load(const std::string& path);
	};
}