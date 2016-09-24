#pragma once
#include <vulkan/vulkan.h>

class VulkanSubmitInfo
{
public:
	VulkanSubmitInfo()
	{
		vkInfo = {};
		vkInfo.pNext = NULL;
		vkInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	}
	
	VulkanSubmitInfo& setWaitSemaphores(std::vector<VkSemaphore>& semaphores)
	{
		vkInfo.waitSemaphoreCount = static_cast<uint32_t>(semaphores.size());
		vkInfo.pWaitSemaphores = semaphores.data();
		return *this;
	}	
	
	VulkanSubmitInfo& setSignalSemaphores(std::vector<VkSemaphore>& semaphores)
	{
		vkInfo.signalSemaphoreCount = static_cast<uint32_t>(semaphores.size());
		vkInfo.pSignalSemaphores = semaphores.data();
		return *this;
	}	
	
	VulkanSubmitInfo& setCommandBuffers(std::vector<VkCommandBuffer>& buffers)
	{
		vkInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());
		vkInfo.pCommandBuffers = buffers.data();
		return *this;
	}	
	
	VulkanSubmitInfo& setDstStageMask(VkPipelineStageFlags* flags)
	{
		vkInfo.pWaitDstStageMask = flags;
		return *this;
	}	

	VkSubmitInfo vkInfo;
};
