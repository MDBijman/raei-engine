#pragma once
#include <vulkan/vulkan.h>

class VulkanPipelineDepthStencilStateCreateInfo
{
public:
	VulkanPipelineDepthStencilStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthTestEnable(VkBool32 state)
	{
		vk.depthTestEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthWriteEnable(VkBool32 state)
	{
		vk.depthWriteEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthCompareOp(VkCompareOp operation)
	{
		vk.depthCompareOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setDepthBoundsTestEnable(VkBool32 state)
	{
		vk.depthBoundsTestEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setFailOp(VkStencilOp operation)
	{
		vk.back.failOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setPassOp(VkStencilOp operation)
	{
		vk.back.passOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setCompareOp(VkCompareOp operation)
	{
		vk.back.compareOp = operation;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setStencilTestEnable(VkBool32 state)
	{
		vk.stencilTestEnable = state;
		return *this;
	}

	VulkanPipelineDepthStencilStateCreateInfo& setFront(VkStencilOpState state)
	{
		vk.front = state;
		return *this;
	}

	VkPipelineDepthStencilStateCreateInfo vk;
};
