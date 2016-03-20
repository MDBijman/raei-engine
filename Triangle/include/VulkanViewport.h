#pragma once
#include <vulkan\vulkan.h>

class VulkanViewport
{
	public:
	VulkanViewport()
	{
		vkViewport = {};
	}

	VulkanViewport& setHeight(float height)
	{
		vkViewport.height = height;
		return *this;
	}

	VulkanViewport& setWidth(float width)
	{
		vkViewport.width = width;
		return *this;
	}

	VulkanViewport& setMinDepth(float depth)
	{
		vkViewport.minDepth = depth;
		return *this;
	}

	VulkanViewport& setMaxDepth(float depth)
	{
		vkViewport.maxDepth = depth;
		return *this;
	}

	VkViewport vkViewport;
};
