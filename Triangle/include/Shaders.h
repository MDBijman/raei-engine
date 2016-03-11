#pragma once

#include <vulkan\vulkan.h>
#include <assert.h>
#include <iostream>

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

	return (char*)shader_code;
}

VkShaderModule loadShader(const char *fileName, VkDevice device, VkShaderStageFlagBits stage)
{
	size_t size = 0;
	const char *shaderCode = readBinaryFile(fileName, &size);
	assert(size > 0);

	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo;
	VkResult err;

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;

	moduleCreateInfo.codeSize = size;
	moduleCreateInfo.pCode = (uint32_t*)shaderCode;
	moduleCreateInfo.flags = 0;
	err = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);
	assert(!err);

	return shaderModule;
}

