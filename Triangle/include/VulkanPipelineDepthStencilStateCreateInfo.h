#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineDepthStencilStateCreateInfo
{
public:
	VulkanPipelineDepthStencilStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthTestEnable(VkBool32 state)
	{
		vkInfo.depthTestEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthWriteEnable(VkBool32 state)
	{
		vkInfo.depthWriteEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthCompareOp(VkCompareOp operation)
	{
		vkInfo.depthCompareOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthBoundsTestEnable(VkBool32 state)
	{
		vkInfo.depthBoundsTestEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setFailOp(VkStencilOp operation)
	{
		vkInfo.back.failOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setPassOp(VkStencilOp operation)
	{
		vkInfo.back.passOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setCompareOp(VkCompareOp operation)
	{
		vkInfo.back.compareOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setStencilTestEnable(VkBool32 state)
	{
		vkInfo.stencilTestEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setFront(VkStencilOpState state)
	{
		vkInfo.front = state;
		return *this;
	}

	VkPipelineDepthStencilStateCreateInfo vkInfo;
};
