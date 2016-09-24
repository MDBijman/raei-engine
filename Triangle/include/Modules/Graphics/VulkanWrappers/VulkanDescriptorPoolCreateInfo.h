#pragma once
#include <vulkan/vulkan.h>

class VulkanDescriptorPoolCreateInfo
{
public:
	VulkanDescriptorPoolCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanDescriptorPoolCreateInfo& setPoolSizes(std::vector<VkDescriptorPoolSize>& poolSizes)
	{
		vkInfo.pPoolSizes = poolSizes.data();
		vkInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		return *this;
	}

	VulkanDescriptorPoolCreateInfo& setMaxSets(uint32_t count)
	{
		vkInfo.maxSets = count;
		return *this;
	}

	VkDescriptorPoolCreateInfo vkInfo;
};