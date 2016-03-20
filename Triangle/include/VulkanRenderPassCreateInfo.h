#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanRenderPassCreateInfo
{
public:
	VulkanRenderPassCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanRenderPassCreateInfo& setAttachments(std::vector<VkAttachmentDescription>& attachments)
	{
		vkInfo.attachmentCount = attachments.size();
		vkInfo.pAttachments = attachments.data();
		return *this;
	}

	VulkanRenderPassCreateInfo& setSubpasses(std::vector<VkSubpassDescription>& subpasses)
	{
		vkInfo.subpassCount = subpasses.size();
		vkInfo.pSubpasses = subpasses.data();
		return *this;
	}

	VulkanRenderPassCreateInfo& setDependencies(std::vector<VkSubpassDependency>& dependencies)
	{
		vkInfo.dependencyCount = dependencies.size();
		vkInfo.pDependencies = dependencies.data();
		return *this;
	}

	VkRenderPassCreateInfo vkInfo;
};