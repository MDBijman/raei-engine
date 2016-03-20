#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineColorBlendAttachmentState
{
public:
	VulkanPipelineColorBlendAttachmentState()
	{
		vkState = {};
	}

	VulkanPipelineColorBlendAttachmentState& setColorWriteMask(VkColorComponentFlags flags)
	{
		vkState.colorWriteMask = flags;
		return *this;
	}

	VulkanPipelineColorBlendAttachmentState& setBlendEnable(VkBool32 blendEnable)
	{
		vkState.blendEnable = blendEnable;
		return *this;
	}

	VkPipelineColorBlendAttachmentState vkState;
};
