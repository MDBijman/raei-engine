#pragma once
#include <vulkan/vulkan.h>
#include "VulkanSampleMask.h"

class VulkanPipelineMultisampleStateCreateInfo
{
public:
	VulkanPipelineMultisampleStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		vk.pNext = NULL;
		vk.pSampleMask = NULL;
		mask = NULL;
	}

	VulkanPipelineMultisampleStateCreateInfo& setSampleMask(VulkanSampleMask m)
	{
		vk.pSampleMask = &mask;
		return *this;
	}

	VulkanPipelineMultisampleStateCreateInfo& setRasterizationSamples(VkSampleCountFlagBits bits)
	{
		vk.rasterizationSamples = bits;
		return *this;
	}

	VkPipelineMultisampleStateCreateInfo vk;

private:
	VkSampleMask mask;
};
