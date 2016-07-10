#pragma once
#include <vulkan\vulkan.h>

class VulkanRenderPass 
{
public:
	VulkanRenderPass() {}
	VulkanRenderPass(VkRenderPass info) : vk(info) {}

	VkRenderPass vk;
};