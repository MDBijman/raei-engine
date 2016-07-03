#pragma once
#include "Graphics\VulkanWrappers\VulkanWrappers.h"
#include <vector>

class Vertices
{
public:
	VkBuffer buf;
	VkDeviceMemory mem;
	VkPipelineVertexInputStateCreateInfo vi;
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
};