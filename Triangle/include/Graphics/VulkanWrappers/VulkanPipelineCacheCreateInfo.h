#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineCacheCreateInfo
{
public:
	VulkanPipelineCacheCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	}

	VkPipelineCacheCreateInfo vkInfo;
};
