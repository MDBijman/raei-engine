#pragma once
#include "Modules/Graphics/Data/Geometry/Vertices.h"
#include "Modules/Graphics/Data/Geometry/Indices.h"

namespace Graphics
{
	namespace Data
	{
		class Mesh
		{
		public:
			Vertices vertices;
			Indices indices;
		};
	}
}
