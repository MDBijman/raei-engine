#pragma once
#include <vulkan\vulkan.h>

class VulkanVertexInputBindingDescription
{
public:
	VulkanVertexInputBindingDescription() {}

	VulkanVertexInputBindingDescription& setBinding(uint32_t b)
	{
		vk.binding = b;
		return *this;
	}

	VulkanVertexInputBindingDescription& setStride(uint32_t s)
	{
		vk.stride = s;
		return *this;
	}

	VulkanVertexInputBindingDescription& setInputRate(VkVertexInputRate i)
	{
		vk.inputRate = i;
		return *this;
	}

	VkVertexInputBindingDescription vk;
};