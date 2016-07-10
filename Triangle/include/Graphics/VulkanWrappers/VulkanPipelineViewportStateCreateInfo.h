#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineViewportStateCreateInfo
{
public:
	VulkanPipelineViewportStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	}

	VulkanPipelineViewportStateCreateInfo& setViewportCount(uint32_t count)
	{
		vk.viewportCount = count;
		return *this;
	}

	VulkanPipelineViewportStateCreateInfo& setScissorCount(uint32_t count)
	{
		vk.scissorCount = count;
		return *this;
	}

	VkPipelineViewportStateCreateInfo vk;
};
