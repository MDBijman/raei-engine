#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include "VulkanPipelineColorBlendAttachmentState.h"

class VulkanPipelineColorBlendStateCreateInfo
{
public:
	VulkanPipelineColorBlendStateCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	}

	VulkanPipelineColorBlendStateCreateInfo& setAttachments(std::vector<VulkanPipelineColorBlendAttachmentState> a)
	{
		attachments.clear();
		std::transform(a.begin(), a.end(), std::back_inserter(attachments), [](VulkanPipelineColorBlendAttachmentState& s) {
			return s.vk;
		});
		vk.attachmentCount = static_cast<uint32_t>(attachments.size());
		vk.pAttachments = attachments.data();
		return *this;
	}

	VkPipelineColorBlendStateCreateInfo vk;

private:
	std::vector<VkPipelineColorBlendAttachmentState> attachments;
};
