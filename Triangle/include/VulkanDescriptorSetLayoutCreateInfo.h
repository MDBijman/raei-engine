#pragma once
#include <vulkan\vulkan.h>

class VulkanDescriptorSetLayoutCreateInfo
{
public:
	VulkanDescriptorSetLayoutCreateInfo()
	{
		vkInfo = {};
		vkInfo.pNext = NULL;
		vkInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	}

	VulkanDescriptorSetLayoutCreateInfo& setBindingCount(uint32_t count)
	{
		vkInfo.bindingCount = count;
		return *this;
	}

	VulkanDescriptorSetLayoutCreateInfo& setBindings(VkDescriptorSetLayoutBinding* bindings)
	{
		vkInfo.pBindings = bindings;
		return *this;
	}

	VkDescriptorSetLayoutCreateInfo vkInfo;
};
