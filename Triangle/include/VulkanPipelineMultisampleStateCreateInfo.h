#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineMultisampleStateCreateInfo
{
public:
	VulkanPipelineMultisampleStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanPipelineMultisampleStateCreateInfo& setSampleMask(VkSampleMask* mask)
	{
		vkInfo.pSampleMask = mask;
		return *this;
	}

	VulkanPipelineMultisampleStateCreateInfo& setRasterizationSamples(VkSampleCountFlagBits bits)
	{
		vkInfo.rasterizationSamples = bits;
		return *this;
	}

	VkPipelineMultisampleStateCreateInfo vkInfo;
};