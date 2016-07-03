#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineViewportStateCreateInfo
{
public:
	VulkanPipelineViewportStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	}

	VulkanPipelineViewportStateCreateInfo& setViewportCount(uint32_t count)
	{
		vkInfo.viewportCount = count;
		return *this;
	}

	VulkanPipelineViewportStateCreateInfo& setScissorCount(uint32_t count)
	{
		vkInfo.scissorCount = count;
		return *this;
	}

	VkPipelineViewportStateCreateInfo vkInfo;
};
