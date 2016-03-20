#pragma once
#include <vulkan\vulkan.h>

class VulkanDescriptorPoolCreateInfo
{
public:
	VulkanDescriptorPoolCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanDescriptorPoolCreateInfo& setPoolSizeCount(uint32_t count)
	{
		vkInfo.poolSizeCount = count;
		return *this;
	}

	VulkanDescriptorPoolCreateInfo& setPoolSizes(VkDescriptorPoolSize* poolSizes)
	{
		vkInfo.pPoolSizes = poolSizes;
		return *this;
	}

	VulkanDescriptorPoolCreateInfo& setMaxSets(uint32_t count)
	{
		vkInfo.maxSets = count;
		return *this;
	}

	VkDescriptorPoolCreateInfo vkInfo;
};