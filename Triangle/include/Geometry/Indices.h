#pragma once
#include "Graphics\VulkanWrappers\VulkanWrappers.h"

class Indices
{
public:
	int count; 
	VkBuffer buf;
	VkDeviceMemory mem;
};
