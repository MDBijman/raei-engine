#pragma once
#include <vulkan\vulkan.h>

class VulkanSubmitInfo
{
public:
	VulkanSubmitInfo()
	{
		vkInfo = {};
		vkInfo.pNext = NULL;
		vkInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	}

	VulkanSubmitInfo& setWaitSemaphoreCount(uint32_t count)
	{
		vkInfo.waitSemaphoreCount = count;
		return *this;
	}

	VulkanSubmitInfo& setSignalSemaphoreCount(uint32_t count)
	{
		vkInfo.signalSemaphoreCount = count;
		return *this;
	}
	
	VulkanSubmitInfo& setCommandBufferCount(uint32_t count)
	{
		vkInfo.commandBufferCount = count;
		return *this;
	}	
	
	VulkanSubmitInfo& setWaitSemaphoresPointer(VkSemaphore* semaphores)
	{
		vkInfo.pWaitSemaphores = semaphores;
		return *this;
	}	
	
	VulkanSubmitInfo& setSignalSemaphoresPointer(VkSemaphore* semaphores)
	{
		vkInfo.pSignalSemaphores = semaphores;
		return *this;
	}	
	
	VulkanSubmitInfo& setCommandBuffersPointer(VkCommandBuffer* buffers)
	{
		vkInfo.pCommandBuffers = buffers;
		return *this;
	}	
	
	VulkanSubmitInfo& setDstStageMaskPointer(VkPipelineStageFlags* flags)
	{
		vkInfo.pWaitDstStageMask = flags;
		return *this;
	}	

	VkSubmitInfo vkInfo;
};
