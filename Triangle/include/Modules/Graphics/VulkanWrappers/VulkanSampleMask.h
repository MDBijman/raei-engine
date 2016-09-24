#pragma once
#include <vulkan/vulkan.h>

class VulkanSampleMask
{
public:
	explicit VulkanSampleMask(VkSampleMask mask) : vk(mask) {}
	VulkanSampleMask() : vk(NULL) {}

	VkSampleMask vk;
};