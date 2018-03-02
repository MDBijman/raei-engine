#pragma once
#include <string>

namespace assets
{
	class manager
	{
		// Path to folder containing fonts, meshes, scripts, etc.
		std::string resources_path;

	public:
		manager(std::string file_path_base) : resources_path(file_path_base)
		{

		}

		
	};
}