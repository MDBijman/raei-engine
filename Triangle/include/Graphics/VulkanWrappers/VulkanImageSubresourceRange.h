#pragma once
#include <vulkan\vulkan.h>

class VulkanImageSubresourceRange
{
public:
	VulkanImageSubresourceRange()
	{
		vkRange = {};
	}

	VulkanImageSubresourceRange& setAspectMask(VkImageAspectFlags aspectMask)
	{
		vkRange.aspectMask = aspectMask;
		return *this;
	}

	VulkanImageSubresourceRange& setBaseMipLevel(uint32_t level)
	{
		vkRange.baseMipLevel = level;
		return *this;
	}

	VulkanImageSubresourceRange& setLevelCount(uint32_t levelCount)
	{
		vkRange.levelCount = levelCount;
		return *this;
	}
	
	VulkanImageSubresourceRange& setBaseArrayLayer(uint32_t baseArrayLayer)
	{
		vkRange.baseArrayLayer = baseArrayLayer;
		return *this;
	}
		
	VulkanImageSubresourceRange& setLayerCount(uint32_t layerCount)
	{
		vkRange.layerCount = layerCount;
		return *this;
	}

	VkImageSubresourceRange vkRange;
};