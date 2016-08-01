#pragma once
#include "Modules\Graphics\Data\Geometry\Mesh.h"

#include <vector>
#include <fstream>

namespace Importers
{
	namespace Obj
	{
		Graphics::Data::Mesh* load(const std::string& path);
	}
}