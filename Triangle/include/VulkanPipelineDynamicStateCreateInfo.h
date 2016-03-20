#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanPipelineDynamicStateCreateInfo
{
public:
	VulkanPipelineDynamicStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanPipelineDynamicStateCreateInfo& setDynamicStates(std::vector<VkDynamicState> states)
	{
		vkInfo.pDynamicStates = states.data();
		vkInfo.dynamicStateCount = states.size();
		return *this;
	}

	VkPipelineDynamicStateCreateInfo vkInfo;
};
