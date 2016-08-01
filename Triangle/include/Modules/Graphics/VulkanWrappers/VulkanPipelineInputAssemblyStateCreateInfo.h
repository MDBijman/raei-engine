#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineInputAssemblyStateCreateInfo
{
public:
	VulkanPipelineInputAssemblyStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vk.pNext = NULL;
	}

	VulkanPipelineInputAssemblyStateCreateInfo& setTopology(VkPrimitiveTopology topology)
	{
		vk.topology = topology;
		return *this;
	}

	VkPipelineInputAssemblyStateCreateInfo vk;
};