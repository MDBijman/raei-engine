#pragma once
#include <vulkan/vulkan.h>

class VulkanPipelineRasterizationStateCreateInfo
{
public:
	VulkanPipelineRasterizationStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	}

	VulkanPipelineRasterizationStateCreateInfo& setPolygonMode(VkPolygonMode mode)
	{
		vk.polygonMode = mode;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setCullMode(VkCullModeFlags mode)
	{
		vk.cullMode = mode;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setFrontFace(VkFrontFace frontFace)
	{
		vk.frontFace = frontFace;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setDepthClampEnable(VkBool32 state)
	{
		vk.depthClampEnable = state;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setRasterizerDiscardEnable(VkBool32 state)
	{
		vk.rasterizerDiscardEnable = state;
		return *this;
	}

	VulkanPipelineRasterizationStateCreateInfo& setDepthBiasEnable(VkBool32 state)
	{
		vk.depthBiasEnable = state;
		return *this;
	}








	VkPipelineRasterizationStateCreateInfo vk;
};