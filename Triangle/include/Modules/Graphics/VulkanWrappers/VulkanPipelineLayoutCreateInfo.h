#pragma once
#include "VulkanDescriptorSetLayout.h"

#include <vulkan/vulkan.h>
#include <vector>

class VulkanPipelineLayoutCreateInfo
{
public:
	VulkanPipelineLayoutCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vk.pNext = NULL;
	}

	VulkanPipelineLayoutCreateInfo& setSetLayouts(std::vector<VulkanDescriptorSetLayout> l)
	{
		layouts.clear();
		std::transform(l.begin(), l.end(), std::back_inserter(layouts), [](VulkanDescriptorSetLayout& r) {
			return r.vk;
		});
		vk.pSetLayouts = layouts.data();
		vk.setLayoutCount = static_cast<uint32_t>(layouts.size());
		return *this;
	}

	VulkanPipelineLayoutCreateInfo& setSetLayouts(VulkanDescriptorSetLayout l)
	{
		layouts.clear();
		layouts.push_back(l.vk);
		vk.pSetLayouts = layouts.data();
		vk.setLayoutCount = static_cast<uint32_t>(layouts.size());
		return *this;
	}

	VkPipelineLayoutCreateInfo vk;

private:
	std::vector<VkDescriptorSetLayout> layouts;
};