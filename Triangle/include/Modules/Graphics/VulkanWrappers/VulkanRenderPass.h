#pragma once
#include <vulkan/vulkan.h>

class VulkanRenderPass 
{
public:
	VulkanRenderPass() : vk(NULL) {}

	explicit VulkanRenderPass(VkRenderPass info) : vk(info) {}

	VkRenderPass vk;
};