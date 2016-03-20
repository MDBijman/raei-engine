#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanPipelineColorBlendStateCreateInfo
{
public:
	VulkanPipelineColorBlendStateCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	}

	VulkanPipelineColorBlendStateCreateInfo& setAttachments(std::vector<VkPipelineColorBlendAttachmentState> attachments)
	{
		vkInfo.attachmentCount = attachments.size();
		vkInfo.pAttachments = attachments.data();
		return *this;
	}

	VkPipelineColorBlendStateCreateInfo vkInfo;
};
