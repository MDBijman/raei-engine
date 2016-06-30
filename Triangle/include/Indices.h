#pragma once
#include "VulkanWrappers.h"

class Indices
{
public:
	int count; 
	VkBuffer buf;
	VkDeviceMemory mem;
};
