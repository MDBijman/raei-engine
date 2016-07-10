#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanDynamicState 
{
public:
	VulkanDynamicState() {}
	VulkanDynamicState(VkDynamicState s) : vk(s) {}

	VkDynamicState vk;
};
