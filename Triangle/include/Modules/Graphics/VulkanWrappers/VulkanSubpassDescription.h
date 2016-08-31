#pragma once
#include <vulkan\vulkan.h>
#include <algorithm>
#include <iterator>

class VulkanSubpassDescription
{
public:
	VulkanSubpassDescription()
	{
		vk = {};
	}

	VulkanSubpassDescription& setFlags(VkSubpassDescriptionFlags flags)
	{
		vk.flags = flags;
		return *this;
	}

	VulkanSubpassDescription& setPipelineBindPoint(VkPipelineBindPoint bindPoint)
	{
		vk.pipelineBindPoint = bindPoint;
		return *this;
	}

	VulkanSubpassDescription& setInputAttachments(std::vector<VulkanAttachmentReference> references)
	{
		inputAttachments.clear();
		std::transform(references.begin(), references.end(), std::back_inserter(inputAttachments), [](VulkanAttachmentReference& r) { 
			return r.vkReference; 
		});
		vk.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
		vk.pInputAttachments = inputAttachments.data();
		return *this;
	}

	VulkanSubpassDescription& setColorAttachments(std::vector<VulkanAttachmentReference> references)
	{
		colorAttachments.clear();
		std::transform(references.begin(), references.end(), std::back_inserter(colorAttachments), [](VulkanAttachmentReference& r) { 
			return r.vkReference; 
		});
		vk.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		vk.pColorAttachments = colorAttachments.data();
		return *this;
	}

	VulkanSubpassDescription& setResolveAttachments(std::vector<VulkanAttachmentReference> references)
	{
		resolveAttachments.clear();
		std::transform(references.begin(), references.end(), std::back_inserter(resolveAttachments), [](VulkanAttachmentReference& r) { 
			return r.vkReference; 
		});
		vk.pResolveAttachments = resolveAttachments.data();
		return *this;
	}

	VulkanSubpassDescription& setDepthStencilAttachment(VulkanAttachmentReference attachment)
	{
		depthStencilAttachment = attachment.vkReference;
		vk.pDepthStencilAttachment = &depthStencilAttachment;
		return *this;
	}

	VulkanSubpassDescription& setPreserveAttachments(std::vector<uint32_t> attachments)
	{
		preserveAttachments = preserveAttachments;
		vk.preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments.size());
		vk.pPreserveAttachments = preserveAttachments.data();
		return *this;
	}

	VkSubpassDescription vk;

private:
	std::vector<VkAttachmentReference> inputAttachments;
	std::vector<VkAttachmentReference> colorAttachments;
	std::vector<VkAttachmentReference> resolveAttachments;
	std::vector<uint32_t> preserveAttachments;
	VkAttachmentReference depthStencilAttachment;
};
