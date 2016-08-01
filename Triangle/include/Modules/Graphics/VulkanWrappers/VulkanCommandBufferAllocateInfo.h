#pragma once
#include <vulkan/vulkan.h>


class VulkanCommandBufferAllocateInfo
{
public:
	VulkanCommandBufferAllocateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanCommandBufferAllocateInfo& setCommandPool(VkCommandPool pool)
	{
		vkInfo.commandPool = pool;
		return *this;
	}

	VulkanCommandBufferAllocateInfo& setCommandBufferCount(uint32_t count)
	{
		vkInfo.commandBufferCount = count;
		return *this;
	}

	VulkanCommandBufferAllocateInfo& setCommandBufferLevel(VkCommandBufferLevel level)
	{
		vkInfo.level = level;
		return *this;
	}

	VkCommandBufferAllocateInfo vkInfo;
};
