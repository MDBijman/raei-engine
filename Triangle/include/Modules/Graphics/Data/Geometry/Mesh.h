#pragma once
#include "Modules\Graphics\Data\Geometry\Vertices.h"
#include "Modules\Graphics\Data\Geometry\Indices.h"

#include <vector>

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
