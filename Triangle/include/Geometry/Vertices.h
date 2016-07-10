#pragma once
#include "Graphics\VulkanWrappers\VulkanWrappers.h"
#include <vector>

class Vertices
{
public:
	Vertices() {}

	VkBuffer buf;
	VkDeviceMemory mem;
	VulkanPipelineVertexInputStateCreateInfo vi;
};