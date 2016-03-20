#pragma once
#include <vulkan\vulkan.h>

class VulkanGraphicsPipelineCreateInfo
{
public:
	VulkanGraphicsPipelineCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanGraphicsPipelineCreateInfo& setLayout(VkPipelineLayout layout)
	{
		vkInfo.layout = layout;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setRenderPass(VkRenderPass renderPass)
	{
		vkInfo.renderPass = renderPass;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setStageCount(uint32_t count)
	{
		vkInfo.stageCount = count;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setVertexInputState(VkPipelineVertexInputStateCreateInfo* info)
	{
		vkInfo.pVertexInputState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setInputAssemblyState(VkPipelineInputAssemblyStateCreateInfo* info)
	{
		vkInfo.pInputAssemblyState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setRasterizationState(VkPipelineRasterizationStateCreateInfo* info)
	{
		vkInfo.pRasterizationState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setColorBlendState(VkPipelineColorBlendStateCreateInfo* info)
	{
		vkInfo.pColorBlendState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setMultisampleState(VkPipelineMultisampleStateCreateInfo* info)
	{
		vkInfo.pMultisampleState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setViewportState(VkPipelineViewportStateCreateInfo* info)
	{
		vkInfo.pViewportState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setDepthStencilState(VkPipelineDepthStencilStateCreateInfo* info)
	{
		vkInfo.pDepthStencilState = info;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setStages(std::vector<VkPipelineShaderStageCreateInfo> info)
	{
		vkInfo.stageCount = info.size();
		vkInfo.pStages = info.data();
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setDynamicState(VkPipelineDynamicStateCreateInfo* info)
	{
		vkInfo.pDynamicState = info;
		return *this;
	}

	VkGraphicsPipelineCreateInfo vkInfo;
};
