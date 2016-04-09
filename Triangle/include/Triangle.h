#pragma once
#include <glm\glm.hpp>
#include <array>
#include "Vertex.h"

struct Triangle
{
	Triangle(Vertex v1, Vertex v2, Vertex v3) : vertices({ v1, v2, v3 }) { }

	std::array<Vertex, 3> vertices;
};
