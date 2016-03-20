#pragma once
#include <vulkan\vulkan.h>

class VulkanWriteDescriptorSet
{
public:
	VulkanWriteDescriptorSet()
	{
		vkDescriptorSet = {};
		vkDescriptorSet.pNext = NULL;
		vkDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	}

	VulkanWriteDescriptorSet& setDstSet(VkDescriptorSet dstSet)
	{
		vkDescriptorSet.dstSet = dstSet;
		return *this;
	}

	VulkanWriteDescriptorSet& setDescriptorCount(uint32_t count)
	{
		vkDescriptorSet.descriptorCount = count;
		return *this;
	}

	VulkanWriteDescriptorSet& setDescriptorType(VkDescriptorType type)
	{
		vkDescriptorSet.descriptorType = type;
		return *this;
	}

	VulkanWriteDescriptorSet& setBufferInfo(VkDescriptorBufferInfo* info)
	{
		vkDescriptorSet.pBufferInfo = info;
		return *this;
	}
	
	VulkanWriteDescriptorSet& setDstBinding(uint32_t dstBinding)
	{
		vkDescriptorSet.dstBinding = dstBinding;
		return *this;
	}

	VkWriteDescriptorSet vkDescriptorSet;
};
