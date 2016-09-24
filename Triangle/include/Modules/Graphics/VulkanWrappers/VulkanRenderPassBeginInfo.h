#pragma once
#include "VulkanWrappers.h"

#include <vulkan/vulkan.h>
#include <vector>

class VulkanRenderPassBeginInfo
{
public:
	VulkanRenderPassBeginInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	}

	VulkanRenderPassBeginInfo& setRenderPass(VulkanRenderPass renderPass)
	{
		vkInfo.renderPass = renderPass.vk;
		return *this;
	}

	VulkanRenderPassBeginInfo& setRenderArea(VkRect2D renderArea)
	{
		vkInfo.renderArea = renderArea;
		return *this;
	}
	
	VulkanRenderPassBeginInfo& setClearValues(std::vector<VkClearValue>& values)
	{
		vkInfo.clearValueCount = static_cast<uint32_t>(values.size());
		vkInfo.pClearValues = values.data();
		return *this;
	}

	VulkanRenderPassBeginInfo& setFramebuffer(VkFramebuffer buffer)
	{
		vkInfo.framebuffer = buffer;
		return *this;
	}

	VkRenderPassBeginInfo vkInfo;
};
