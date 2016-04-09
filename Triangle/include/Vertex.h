#pragma once
#include <glm\glm.hpp>

struct Vertex
{
	Vertex(glm::vec3 pos) : pos(pos) {}
	glm::vec3 pos;
};
