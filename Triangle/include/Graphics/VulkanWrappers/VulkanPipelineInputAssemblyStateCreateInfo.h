#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineInputAssemblyStateCreateInfo
{
public:
	VulkanPipelineInputAssemblyStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanPipelineInputAssemblyStateCreateInfo& setTopology(VkPrimitiveTopology topology)
	{
		vkInfo.topology = topology;
		return *this;
	}

	VkPipelineInputAssemblyStateCreateInfo vkInfo;
};