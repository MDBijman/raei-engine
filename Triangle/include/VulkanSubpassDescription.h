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

	VulkanSubpassDescription& setInputAttachments(std::vector<VkAttachmentReference>& inputAttachments)
	{
		vkDescription.inputAttachmentCount = inputAttachments.size();
		vkDescription.pInputAttachments = inputAttachments.data();
		return *this;
	}

	VulkanSubpassDescription& setColorAttachments(std::vector<VkAttachmentReference>& colorAttachments)
	{
		vkDescription.colorAttachmentCount = colorAttachments.size();
		vkDescription.pColorAttachments = colorAttachments.data();
		return *this;
	}

	VulkanSubpassDescription& setResolveAttachments(std::vector<VkAttachmentReference>& resolveAttachments)
	{
		vkDescription.pResolveAttachments = resolveAttachments.data();
		return *this;
	}

	VulkanSubpassDescription& setDepthStencilAttachment(VkAttachmentReference& depthStencilAttachment)
	{
		vkDescription.pDepthStencilAttachment = &depthStencilAttachment;
		return *this;
	}

	VulkanSubpassDescription& setPreserveAttachments(std::vector<uint32_t>& preserveAttachments)
	{
		vkDescription.preserveAttachmentCount = preserveAttachments.size();
		vkDescription.pPreserveAttachments = preserveAttachments.data();
		return *this;
	}

	VkSubpassDescription vkDescription;
};
