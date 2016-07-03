#pragma once
#include <vulkan\vulkan.h>

class VulkanAttachmentDescription
{
public:
	VulkanAttachmentDescription()
	{
		vkDescription = {};
	}

	VulkanAttachmentDescription& setFlags(VkAttachmentDescriptionFlags flags)
	{
		vkDescription.flags = flags;
		return *this;
	}

	VulkanAttachmentDescription& setFormat(VkFormat format)
	{
		vkDescription.format = format;
		return *this;
	}

	VulkanAttachmentDescription& setSamples(VkSampleCountFlagBits samples)
	{
		vkDescription.samples = samples;
		return *this;
	}

	VulkanAttachmentDescription& setLoadOp(VkAttachmentLoadOp loadOp)
	{
		vkDescription.loadOp = loadOp;
		return *this;
	}

	VulkanAttachmentDescription& setStoreOp(VkAttachmentStoreOp storeOp)
	{
		vkDescription.storeOp = storeOp;
		return *this;
	}

	VulkanAttachmentDescription& setStencilLoadOp(VkAttachmentLoadOp stencilLoadOp)
	{
		vkDescription.stencilLoadOp = stencilLoadOp;
		return *this;
	}

	VulkanAttachmentDescription& setStencilStoreOp(VkAttachmentStoreOp stencilStoreOp)
	{
		vkDescription.stencilStoreOp = stencilStoreOp;
		return *this;
	}

	VulkanAttachmentDescription& setInitialLayout(VkImageLayout initialLayout)
	{
		vkDescription.initialLayout = initialLayout;
		return *this;
	}

	VulkanAttachmentDescription& setFinalLayout(VkImageLayout finalLayout)
	{
		vkDescription.finalLayout = finalLayout;
		return *this;
	}

	VkAttachmentDescription vkDescription;
};
