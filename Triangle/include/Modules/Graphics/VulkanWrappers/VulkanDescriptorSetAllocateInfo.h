#pragma once
#include <vulkan/vulkan.h>
#include <algorithm>
#include <iterator>
#include "VulkanDescriptorSetLayout.h"

class VulkanDescriptorSetAllocateInfo
{
public:
	VulkanDescriptorSetAllocateInfo()
	{
		vk.pNext = NULL;
		vk.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	}

	VulkanDescriptorSetAllocateInfo& setDescriptorPool(VkDescriptorPool pool)
	{
		vk.descriptorPool = pool;
		return *this;
	}

	VulkanDescriptorSetAllocateInfo& setSetLayouts(std::vector<VulkanDescriptorSetLayout> l)
	{
		layouts.clear();
		std::transform(l.begin(), l.end(), std::back_inserter(layouts), [](VulkanDescriptorSetLayout& sl) {
			return sl.vk;
		});
		vk.pSetLayouts = layouts.data();
		vk.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		return *this;
	}

	VulkanDescriptorSetAllocateInfo& setSetLayouts(VulkanDescriptorSetLayout l)
	{
		layouts.clear();
		layouts.push_back(l.vk);
		vk.pSetLayouts = layouts.data();
		vk.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		return *this;
	}

	VkDescriptorSetAllocateInfo vk;

private:
	std::vector<VkDescriptorSetLayout> layouts;

};
