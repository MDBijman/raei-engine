#pragma once
#include <vulkan\vulkan.h>

class Uniform
{
public:
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDescriptorBufferInfo descriptor;
};
