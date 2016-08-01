#pragma once
#include <vulkan\vulkan.h>

class VulkanPipelineColorBlendAttachmentState
{
public:
	VulkanPipelineColorBlendAttachmentState()
	{
		vk = {};
	}

	VulkanPipelineColorBlendAttachmentState& setColorWriteMask(VkColorComponentFlags flags)
	{
		vk.colorWriteMask = flags;
		return *this;
	}

	VulkanPipelineColorBlendAttachmentState& setBlendEnable(VkBool32 blendEnable)
	{
		vk.blendEnable = blendEnable;
		return *this;
	}

	VkPipelineColorBlendAttachmentState vk;
};
