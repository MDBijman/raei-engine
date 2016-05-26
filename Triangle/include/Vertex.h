#pragma once
#include <glm\glm.hpp>

struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) : pos(pos), uv(uv), normal(normal) {}
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
};
