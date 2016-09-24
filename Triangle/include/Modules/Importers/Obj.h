#pragma once
#include "Modules/Graphics/Data/Geometry/Mesh.h"

namespace Importers
{
	namespace Obj
	{
		Graphics::Data::Mesh* load(const std::string& path);
	}
}