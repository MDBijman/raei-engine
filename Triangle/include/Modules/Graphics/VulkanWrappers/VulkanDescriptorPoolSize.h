#pragma once
#include <vulkan\vulkan.h>

class VulkanDescriptorPoolSize
{
public:
	VulkanDescriptorPoolSize()
	{
		vkPoolSize = {};
	}

	VulkanDescriptorPoolSize& setType(VkDescriptorType type)
	{
		vkPoolSize.type = type;
		return *this;
	}

	VulkanDescriptorPoolSize& setDescriptorCount(uint32_t count)
	{
		vkPoolSize.descriptorCount = count;
		return *this;
	}

	VkDescriptorPoolSize vkPoolSize;
};
