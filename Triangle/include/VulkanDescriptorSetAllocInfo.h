#pragma once
#include <vulkan\vulkan.h>

class VulkanDescriptorSetAllocInfo
{
public:
	VulkanDescriptorSetAllocInfo()
	{
		vkInfo.pNext = NULL;
		vkInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	}

	VulkanDescriptorSetAllocInfo& setDescriptorPool(VkDescriptorPool pool)
	{
		vkInfo.descriptorPool = pool;
		return *this;
	}

	VulkanDescriptorSetAllocInfo& setDescriptorSetCount(uint32_t count)
	{
		vkInfo.descriptorSetCount = count;
		return *this;
	}

	VulkanDescriptorSetAllocInfo& setSetLayoutsPointer(VkDescriptorSetLayout* layouts)
	{
		vkInfo.pSetLayouts = layouts;
		return *this;
	}

	VkDescriptorSetAllocateInfo vkInfo;
};
