#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include "VulkanDynamicState.h"

class VulkanPipelineDynamicStateCreateInfo
{
public:
	VulkanPipelineDynamicStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		vk.pNext = NULL;
	}

	VulkanPipelineDynamicStateCreateInfo& setDynamicStates(std::vector<VulkanDynamicState> s)
	{
		states.clear();
		std::transform(s.begin(), s.end(), std::back_inserter(states), [](VulkanDynamicState& s) {
			return s.vk;
		});
		vk.pDynamicStates = states.data();
		vk.dynamicStateCount = static_cast<uint32_t>(states.size());
		return *this;
	}

	VkPipelineDynamicStateCreateInfo vk;

private:
	std::vector<VkDynamicState> states;
};
