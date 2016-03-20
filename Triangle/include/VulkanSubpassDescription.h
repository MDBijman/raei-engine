#pragma once
#include <vulkan\vulkan.h>

class VulkanSubpassDescription
{
public:
	VulkanSubpassDescription()
	{
		vkDescription = {};
	}

	VulkanSubpassDescription& setFlags(VkSubpassDescriptionFlags flags)
	{
		vkDescription.flags = flags;
		return *this;
	}

	VulkanSubpassDescription& setPipelineBindPoint(VkPipelineBindPoint bindPoint)
	{
		vkDescription.pipelineBindPoint = bindPoint;
		return *this;
	}

	VulkanSubpassDescription& setInputAttachmentCount(uint32_t count)
	{
		vkDescription.inputAttachmentCount = count;
		return *this;
	}

	VulkanSubpassDescription& setInputAttachmentsPointer(VkAttachmentReference* reference)
	{
		vkDescription.pInputAttachments = reference;
		return *this;
	}

	VulkanSubpassDescription& setColorAttachmentCount(uint32_t count)
	{
		vkDescription.colorAttachmentCount = count;
		return *this;
	}

	VulkanSubpassDescription& setColorAttachmentsPointer(VkAttachmentReference* reference)
	{
		vkDescription.pColorAttachments = reference;
		return *this;
	}

	VulkanSubpassDescription& setResolveAttachmentsPointer(VkAttachmentReference* reference)
	{
		vkDescription.pResolveAttachments = reference;
		return *this;
	}

	VulkanSubpassDescription& setDepthStencilAttachmentPointer(VkAttachmentReference* reference)
	{
		vkDescription.pDepthStencilAttachment = reference;
		return *this;
	}

	VulkanSubpassDescription& setPreserveAttachmentCount(uint32_t count)
	{
		vkDescription.preserveAttachmentCount = count;
		return *this;
	}

	VulkanSubpassDescription& setPreserveAttachmentsPointer(const uint32_t* preserveAttachments)
	{
		vkDescription.pPreserveAttachments = preserveAttachments;
		return *this;
	}

	VkSubpassDescription vkDescription;
};
