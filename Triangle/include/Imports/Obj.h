#pragma once
#include "Imports\Parser.h"
#include "Geometry\Vertex.h"
#include "Geometry\Mesh.h"

#include <vector>
#include <fstream>


namespace Importers
{
	namespace Obj
	{
		Mesh* load(std::string path);
	}
};