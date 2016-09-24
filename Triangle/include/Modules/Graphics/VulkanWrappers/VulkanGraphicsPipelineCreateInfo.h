#pragma once
#include <vulkan/vulkan.h>
#include "VulkanPipelineLayout.h"
#include "VulkanPipelineVertexInputStateCreateInfo.h"
#include "VulkanPipelineInputAssemblyStateCreateInfo.h"
#include "VulkanPipelineRasterizationStateCreateInfo.h"
#include "VulkanPipelineColorBlendStateCreateInfo.h"
#include "VulkanPipelineMultisampleStateCreateInfo.h"
#include "VulkanPipelineViewportStateCreateInfo.h"
#include "VulkanPipelineDepthStencilStateCreateInfo.h"
#include "VulkanPipelineShaderStageCreateInfo.h"
#include "VulkanPipelineDynamicStateCreateInfo.h"

class VulkanGraphicsPipelineCreateInfo
{
public:
	VulkanGraphicsPipelineCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanGraphicsPipelineCreateInfo& setLayout(VulkanPipelineLayout layout)
	{
		pipelineLayout = layout.vk;
		vkInfo.layout = pipelineLayout;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setRenderPass(VulkanRenderPass rp)
	{
		renderPass = rp.vk;
		vkInfo.renderPass = renderPass;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setStageCount(uint32_t count)
	{
		vkInfo.stageCount = count;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setVertexInputState(VulkanPipelineVertexInputStateCreateInfo info)
	{
		vertexInputInfo = info.vk;
		vkInfo.pVertexInputState = &vertexInputInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setInputAssemblyState(VulkanPipelineInputAssemblyStateCreateInfo info)
	{
		inputAssemblyInfo = info.vk;
		vkInfo.pInputAssemblyState = &inputAssemblyInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setRasterizationState(VulkanPipelineRasterizationStateCreateInfo info)
	{
		rasterizationInfo = info.vk;
		vkInfo.pRasterizationState = &rasterizationInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setColorBlendState(VulkanPipelineColorBlendStateCreateInfo info)
	{
		colorBlendInfo = info.vk;
		vkInfo.pColorBlendState = &colorBlendInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setMultisampleState(VulkanPipelineMultisampleStateCreateInfo info)
	{
		multisampleInfo = info.vk;
		vkInfo.pMultisampleState = &multisampleInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setViewportState(VulkanPipelineViewportStateCreateInfo info)
	{
		viewportInfo = info.vk;
		vkInfo.pViewportState = &viewportInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setDepthStencilState(VulkanPipelineDepthStencilStateCreateInfo info)
	{
		depthStencilInfo = info.vk;
		vkInfo.pDepthStencilState = &depthStencilInfo;
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setStages(std::vector<VulkanPipelineShaderStageCreateInfo> info)
	{
		std::transform(info.begin(), info.end(), std::back_inserter(shaderStageInfo), [](VulkanPipelineShaderStageCreateInfo& a) {
			return a.vk;
		});
		vkInfo.stageCount = static_cast<uint32_t>(shaderStageInfo.size());
		vkInfo.pStages = shaderStageInfo.data();
		return *this;
	}

	VulkanGraphicsPipelineCreateInfo& setDynamicState(VulkanPipelineDynamicStateCreateInfo info)
	{
		dynamicStateInfo = info.vk;
		vkInfo.pDynamicState = &dynamicStateInfo;
		return *this;
	}

	VkGraphicsPipelineCreateInfo vkInfo;

private:
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfo;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;

};
