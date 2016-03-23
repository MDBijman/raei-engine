#pragma once
#include <vulkan\vulkan.h>

class VulkanClearValue
{
public:
	VulkanClearValue() {}
	VulkanClearValue(VkClearValue vulkanValue) : vkValue(vulkanValue) {}

	VulkanClearValue& setColor(VkClearColorValue color)
	{
		vkValue.color = color;
		return *this;
	}

	VulkanClearValue& setDepthStencil(VkClearDepthStencilValue stencil)
	{
		vkValue.depthStencil = stencil;
		return *this;
	}

	VkClearValue vkValue;
};