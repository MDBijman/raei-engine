#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <tuple>
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