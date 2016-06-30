#include "StateBucket.h"
#include "VulkanWrappers.h"

namespace Graphics
{
	StateBucket::StateBucket()
	{
		instance = std::make_unique<VulkanInstance>("triangle");
		physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance->getPhysicalDevices()->at(0));

		auto queueProperties = physicalDevice->queueFamilyProperties();
		uint32_t graphicsQueueIndex;
		for (graphicsQueueIndex = 0; graphicsQueueIndex < queueProperties->size(); graphicsQueueIndex++)
		{
			if (queueProperties->at(graphicsQueueIndex).queueFlags & VK_QUEUE_GRAPHICS_BIT)
				break;
		}
		assert(graphicsQueueIndex < queueProperties->size());

		device = std::make_unique<VulkanDevice>(physicalDevice->vkPhysicalDevice, graphicsQueueIndex);
		swapchain = std::make_unique<VulkanSwapChain>(instance->vkInstance, physicalDevice->vkPhysicalDevice, device->vkDevice);
		queue = std::make_unique<VulkanQueue>(device->queueAt(graphicsQueueIndex));

		// Find supported depth format
		// We prefer 24 bits of depth and 8 bits of stencil, but that may not be supported by all implementations
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
		for (auto& format : depthFormats)
		{
			if (physicalDevice->formatProperties(format).optimalTilingFeatures && VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				depthFormat = format;
				break;
			}
		}


	}
}
