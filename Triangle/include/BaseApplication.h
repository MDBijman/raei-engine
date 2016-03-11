#pragma once

#include <vulkan\vulkan.h>
#include <assert.h>
#include <memory>
#include <vector>
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"

namespace VA
{
	class BaseApplication
	{
	protected:
		// Returns the index of the first queueFamily matching the given FlagBits
		uint32_t getQueueIndexOfType(VkQueueFlagBits bits)
		{
			uint32_t queueIndex;
			for (queueIndex = 0; queueIndex < queueProperties.size(); queueIndex++)
			{
				if (queueProperties.at(queueIndex).queueFlags & bits)
					break;
			}
			assert(queueIndex < queueProperties.size());
			return queueIndex;
		}

		void beginCommandBuffer(VkCommandBuffer& buffer, VkCommandBufferBeginInfo& info)
		{
			VkResult error = vkBeginCommandBuffer(buffer, &info);
			assert(!error);
		}

		void submitToQueue(VkQueue& queue, uint32_t count, VkSubmitInfo& info, VkFence& fence)
		{
			VkResult error = vkQueueSubmit(queue, count, &info, fence);
			assert(!error);
		}

		void submitToQueue(VkQueue& queue, uint32_t count, VkSubmitInfo& info)
		{
			VkResult error = vkQueueSubmit(queue, count, &info, VK_NULL_HANDLE);
			assert(!error);
		}
	private:
	};
}