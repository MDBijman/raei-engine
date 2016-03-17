#pragma once

#include <assert.h>
#include <vulkan\vulkan.h>

class VulkanQueue
{
public:
	VulkanQueue(VkQueue q) : queue(q) {}

	void submit(uint32_t count, VkSubmitInfo& vkInfo)
	{
		VkResult error = vkQueueSubmit(queue, count, &vkInfo, VK_NULL_HANDLE);
		assert(!error);
	}

	void submit(uint32_t count, VkSubmitInfo& vkInfo, VkFence& fence)
	{
		VkResult error = vkQueueSubmit(queue, count, &vkInfo, fence);
		assert(!error);
	}

	void waitIdle()
	{
		VkResult error = vkQueueWaitIdle(queue);
		assert(!error);
	}


	operator VkQueue() const { return queue; }
	VkQueue& getVkQueue()
	{
		return queue;
	}
private:
	VkQueue queue;
};
