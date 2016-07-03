#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineRasterizationStateCreateInfo
{
public:
	VulkanPipelineRasterizationStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	}

	VulkanPipelineRasterizationStateCreateInfo& setPolygonMode(VkPolygonMode mode)
	{
		vkInfo.polygonMode = mode;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setCullMode(VkCullModeFlags mode)
	{
		vkInfo.cullMode = mode;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setFrontFace(VkFrontFace frontFace)
	{
		vkInfo.frontFace = frontFace;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setDepthClampEnable(VkBool32 state)
	{
		vkInfo.depthClampEnable = state;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setRasterizerDiscardEnable(VkBool32 state)
	{
		vkInfo.rasterizerDiscardEnable = state;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setDepthBiasEnable(VkBool32 state)
	{
		vkInfo.depthBiasEnable = state;
		return *this;
	}








	VkPipelineRasterizationStateCreateInfo vkInfo;
};