#pragma once
#include <vulkan/vulkan.h>

namespace Importers
{
	namespace Shader
	{
		char* readBinaryFile(const char *filename, size_t *psize);

		VkShaderModule load(const char *fileName, VkDevice device);
	}
}