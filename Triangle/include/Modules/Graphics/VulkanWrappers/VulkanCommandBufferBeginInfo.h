#pragma once
#include <vulkan/vulkan.h>

class VulkanCommandBufferBeginInfo
{
public:
	VulkanCommandBufferBeginInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkInfo.pNext = NULL;
	}


	VkCommandBufferBeginInfo vkInfo;
};
