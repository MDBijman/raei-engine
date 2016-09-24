#pragma once
#include <vulkan/vulkan.h>

class VulkanCommandPoolCreateInfo
{
public:
	VulkanCommandPoolCreateInfo()
	{
		vkInfo = {};
		vkInfo.pNext = NULL;
		vkInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	}

	VulkanCommandPoolCreateInfo& setFlags(VkCommandPoolCreateFlags flags)
	{
		vkInfo.flags = flags;
		return *this;
	}

	VulkanCommandPoolCreateInfo& setQueueFamilyIndex(uint32_t index)
	{
		vkInfo.queueFamilyIndex = index;
		return *this;
	}

	VkCommandPoolCreateInfo vkInfo;
};