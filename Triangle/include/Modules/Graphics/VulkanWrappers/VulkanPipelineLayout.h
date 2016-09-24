#pragma once
#include <vulkan/vulkan.h>

class VulkanPipelineLayout
{
public:
	VulkanPipelineLayout(): vk(NULL) {}

	explicit VulkanPipelineLayout(VkPipelineLayout info) : vk(info) {}

	VkPipelineLayout vk;
};