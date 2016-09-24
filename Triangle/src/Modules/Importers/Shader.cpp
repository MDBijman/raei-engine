#include "Modules/Importers/Shader.h"
#include <cstdio>
#include <cassert>

namespace Importers
{
	namespace Shader
	{
		char* readBinaryFile(const char *filename, size_t *psize)
		{
			long int size;
			size_t retval;
			void *shader_code;

			FILE *fp = fopen(filename, "rb");
			if (!fp) return NULL;

			fseek(fp, 0L, SEEK_END);
			size = ftell(fp);

			fseek(fp, 0L, SEEK_SET);

			shader_code = malloc(size);
			retval = fread(shader_code, size, 1, fp);
			assert(retval == 1);

			*psize = size;

			return static_cast<char*>(shader_code);
		}

		VkShaderModule load(const char *fileName, VkDevice device)
		{
			size_t size = 0;
			const char *shaderCode = readBinaryFile(fileName, &size);
			assert(size > 0);

			VkShaderModule shaderModule;
			VkShaderModuleCreateInfo moduleCreateInfo;
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.pNext = NULL;
			moduleCreateInfo.codeSize = size;
			moduleCreateInfo.pCode = (uint32_t*)shaderCode;
			moduleCreateInfo.flags = 0;
			VkResult err = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);
			assert(!err);

			return shaderModule;
		}
	}
}
