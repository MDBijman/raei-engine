#pragma once
#include "VulkanWrappers.h"

#include <vulkan\vulkan.h>
#include <vector>
#include <algorithm>
#include <iterator>

class VulkanRenderPassCreateInfo
{
public:
	VulkanRenderPassCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vk.pNext = NULL;
	}

	VulkanRenderPassCreateInfo& setAttachments(std::vector<VulkanAttachmentDescription> a)
	{
		attachments.clear();
		std::transform(a.begin(), a.end(), std::back_inserter(attachments), [](VulkanAttachmentDescription& desc) {
			return desc.vk;
		});
		vk.attachmentCount = static_cast<uint32_t>(attachments.size());
		vk.pAttachments = attachments.data();
		return *this;
	}

	VulkanRenderPassCreateInfo& setSubpasses(std::vector<VulkanSubpassDescription> s)
	{
		subpasses.clear();
		std::transform(s.begin(), s.end(), std::back_inserter(subpasses), [](VulkanSubpassDescription& desc) {
			return desc.vk;
		});
		vk.subpassCount = static_cast<uint32_t>(subpasses.size());
		vk.pSubpasses = subpasses.data();
		return *this;
	}

	VulkanRenderPassCreateInfo& setDependencies(std::vector<VulkanSubpassDependency> d)
	{
		dependencies.clear();
		std::transform(d.begin(), d.end(), std::back_inserter(dependencies), [](VulkanSubpassDependency& desc) {
			return desc.vk;
		});
		vk.dependencyCount = static_cast<uint32_t>(dependencies.size());
		vk.pDependencies = dependencies.data();
		return *this;
	}

	VkRenderPassCreateInfo vk;

private:
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDescription> subpasses;
	std::vector<VkSubpassDependency> dependencies;
};