#pragma once
#include <vulkan\vulkan.h>

class VulkanAttachmentDescription
{
public:
	VulkanAttachmentDescription()
	{
		vk = {};
	}

	VulkanAttachmentDescription& setFlags(VkAttachmentDescriptionFlags flags)
	{
		vk.flags = flags;
		return *this;
	}

	VulkanAttachmentDescription& setFormat(VkFormat format)
	{
		vk.format = format;
		return *this;
	}

	VulkanAttachmentDescription& setSamples(VkSampleCountFlagBits samples)
	{
		vk.samples = samples;
		return *this;
	}

	VulkanAttachmentDescription& setLoadOp(VkAttachmentLoadOp loadOp)
	{
		vk.loadOp = loadOp;
		return *this;
	}

	VulkanAttachmentDescription& setStoreOp(VkAttachmentStoreOp storeOp)
	{
		vk.storeOp = storeOp;
		return *this;
	}

	VulkanAttachmentDescription& setStencilLoadOp(VkAttachmentLoadOp stencilLoadOp)
	{
		vk.stencilLoadOp = stencilLoadOp;
		return *this;
	}

	VulkanAttachmentDescription& setStencilStoreOp(VkAttachmentStoreOp stencilStoreOp)
	{
		vk.stencilStoreOp = stencilStoreOp;
		return *this;
	}

	VulkanAttachmentDescription& setInitialLayout(VkImageLayout initialLayout)
	{
		vk.initialLayout = initialLayout;
		return *this;
	}

	VulkanAttachmentDescription& setFinalLayout(VkImageLayout finalLayout)
	{
		vk.finalLayout = finalLayout;
		return *this;
	}

	VkAttachmentDescription vk;
};
