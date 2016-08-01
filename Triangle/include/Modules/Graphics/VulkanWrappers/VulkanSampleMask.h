#pragma once
#include <vulkan\vulkan.h>

class VulkanSampleMask
{
public:
	VulkanSampleMask(VkSampleMask mask) : vk(mask) {}
	VulkanSampleMask() 
	{
		vk = NULL;
	}

	VkSampleMask vk;
};