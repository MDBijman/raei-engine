#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineLayoutCreateInfo
{
public:
	VulkanPipelineLayoutCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanPipelineLayoutCreateInfo& setSetLayoutCount(uint32_t count)
	{
		vkInfo.setLayoutCount = count;
		return *this;
	}

	VulkanPipelineLayoutCreateInfo& setSetLayouts(VkDescriptorSetLayout* layouts)
	{
		vkInfo.pSetLayouts = layouts;
		return *this;
	}

	VkPipelineLayoutCreateInfo vkInfo;
};