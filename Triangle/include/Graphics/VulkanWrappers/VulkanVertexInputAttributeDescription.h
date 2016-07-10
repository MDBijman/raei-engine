#pragma once
#include <vulkan\vulkan.h>

class VulkanVertexInputAttributeDescription
{
public:
	VulkanVertexInputAttributeDescription() {}

	VulkanVertexInputAttributeDescription& setBinding(uint32_t b)
	{
		vk.binding = b;
		return *this;
	}

	VulkanVertexInputAttributeDescription& setLocation(uint32_t l)
	{
		vk.location = l;
		return *this;
	}

	VulkanVertexInputAttributeDescription& setFormat(VkFormat f)
	{
		vk.format = f;
		return *this;
	}

	VulkanVertexInputAttributeDescription& setOffset(uint32_t o)
	{
		vk.offset = o;
		return *this;
	}

	VkVertexInputAttributeDescription vk;
};