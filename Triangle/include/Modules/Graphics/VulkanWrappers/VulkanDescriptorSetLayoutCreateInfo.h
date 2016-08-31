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

	VulkanDescriptorSetLayoutCreateInfo& setBindings(std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		vkInfo.pBindings = bindings.data();
		vkInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		return *this;
	}

	VkDescriptorSetLayoutCreateInfo vkInfo;
};
