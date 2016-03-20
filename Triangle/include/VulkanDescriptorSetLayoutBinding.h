#pragma once
#include <vulkan\vulkan.h>

class VulkanDescriptorSetLayoutBinding
{
public:
	VulkanDescriptorSetLayoutBinding()
	{
		vkLayoutBinding = {};
	}

	VulkanDescriptorSetLayoutBinding& setDescriptorType(VkDescriptorType type)
	{
		vkLayoutBinding.descriptorType = type;
		return *this;
	}

	VulkanDescriptorSetLayoutBinding& setDescriptorCount(uint32_t count)
	{
		vkLayoutBinding.descriptorCount = count;
		return *this;
	}

	VulkanDescriptorSetLayoutBinding& setStageFlags(VkShaderStageFlags flags)
	{
		vkLayoutBinding.stageFlags = flags;
		return *this;
	}

	VulkanDescriptorSetLayoutBinding& setImmutableSamples(VkSampler* samplers)
	{
		vkLayoutBinding.pImmutableSamplers = samplers;
		return *this;
	}

	VkDescriptorSetLayoutBinding vkLayoutBinding;
};