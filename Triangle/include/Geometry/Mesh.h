#pragma once
#include "Geometry\Vertex.h"

#include <vector>

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};
