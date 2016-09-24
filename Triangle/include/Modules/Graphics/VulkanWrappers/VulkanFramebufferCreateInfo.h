#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class VulkanFramebufferCreateInfo
{
public:
	VulkanFramebufferCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanFramebufferCreateInfo& setRenderPass(VkRenderPass renderPass)
	{
		vkInfo.renderPass = renderPass;
		return *this;
	}
	
	VulkanFramebufferCreateInfo& setAttachments(std::vector<VkImageView>& attachments)
	{
		vkInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		vkInfo.pAttachments = attachments.data();
		return *this;
	}
	
	VulkanFramebufferCreateInfo& setWidth(uint32_t width)
	{
		vkInfo.width = width;
		return *this;
	}
		
	VulkanFramebufferCreateInfo& setHeight(uint32_t height)
	{
		vkInfo.height = height;
		return *this;
	}

	VulkanFramebufferCreateInfo& setLayers(uint32_t layers)
	{
		vkInfo.layers = layers;
		return *this;
	}

	VkFramebufferCreateInfo vkInfo;
};
