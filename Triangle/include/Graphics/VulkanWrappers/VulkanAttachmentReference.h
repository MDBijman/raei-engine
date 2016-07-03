#pragma once
#include <vulkan\vulkan.h>

class VulkanAttachmentReference
{
public:
	VulkanAttachmentReference()
	{
		vkReference = {};
	}

	VulkanAttachmentReference& setAttachment(uint32_t index)
	{
		vkReference.attachment = index;
		return *this;
	}

	VulkanAttachmentReference& setImageLayout(VkImageLayout layout)
	{
		vkReference.layout = layout;
		return *this;
	}

	VkAttachmentReference vkReference;
};
