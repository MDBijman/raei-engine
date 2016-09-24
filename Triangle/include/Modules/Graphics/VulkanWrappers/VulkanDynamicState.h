#pragma once
#include <vulkan/vulkan.h>

class VulkanDynamicState 
{
public:
	VulkanDynamicState() : vk() {}
	explicit VulkanDynamicState(VkDynamicState s) : vk(s) {}

	VkDynamicState vk;
};
