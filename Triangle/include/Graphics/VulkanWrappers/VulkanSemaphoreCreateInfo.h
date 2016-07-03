#pragma once
#include <vulkan\vulkan.h>

class VulkanSemaphoreCreateInfo
{
public:
	VulkanSemaphoreCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VkSemaphoreCreateInfo vkInfo;
};
