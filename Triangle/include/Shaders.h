#pragma once
#include <vulkan\vulkan.h>
#include <assert.h>
#include <iostream>

char* readBinaryFile(const char *filename, size_t *psize);

VkShaderModule loadShader(const char *fileName, VkDevice device, VkShaderStageFlagBits stage);