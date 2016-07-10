#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineLayout
{
public:
	VulkanPipelineLayout() {}
	VulkanPipelineLayout(VkPipelineLayout info) : vk(info) {}

	VkPipelineLayout vk;
};