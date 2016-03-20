#pragma once
#include <vulkan\vulkan.h>
#include <vector>

class VulkanRenderPassBeginInfo
{
public:
	VulkanRenderPassBeginInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	}

	VulkanRenderPassBeginInfo& setRenderPass(VkRenderPass renderPass)
	{
		vkInfo.renderPass = renderPass;
		return *this;
	}

	VulkanRenderPassBeginInfo& setRenderArea(VkRect2D renderArea)
	{
		vkInfo.renderArea = renderArea;
		return *this;
	}
	
	VulkanRenderPassBeginInfo& setClearValues(std::vector<VkClearValue>& values)
	{
		vkInfo.clearValueCount = values.size();
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
