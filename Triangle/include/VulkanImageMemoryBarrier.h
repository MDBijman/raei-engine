#pragma once
#include <vulkan\vulkan.h>

class VulkanImageMemoryBarrier
{
public:
	VulkanImageMemoryBarrier()
	{
		vkBarrier = {};
		vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		vkBarrier.pNext = NULL;
	}

	VulkanImageMemoryBarrier& setSrcAccessMask(VkAccessFlags src)
	{
		vkBarrier.srcAccessMask = src;
		return *this;
	}
	
	VulkanImageMemoryBarrier& setDstAccessMask(VkAccessFlags dst)
	{
		vkBarrier.dstAccessMask = dst;
		return *this;
	}
	
	VulkanImageMemoryBarrier& setOldLayout(VkImageLayout layout)
	{
		vkBarrier.oldLayout = layout;
		return *this;
	}

	VulkanImageMemoryBarrier& setNewLayout(VkImageLayout layout)
	{
		vkBarrier.newLayout = layout;
		return *this;
	}

	VulkanImageMemoryBarrier& setSrcQueueFamilyIndex(uint32_t index)
	{
		vkBarrier.srcQueueFamilyIndex = index;
		return *this;
	}

	VulkanImageMemoryBarrier& setDstQueueFamilyIndex(uint32_t index)
	{
		vkBarrier.dstQueueFamilyIndex = index;
		return *this;
	}

	VulkanImageMemoryBarrier& setSubresourceRange(VkImageSubresourceRange range)
	{
		vkBarrier.subresourceRange = range;
		return *this;
	}

	VulkanImageMemoryBarrier& setImage(VkImage image)
	{
		vkBarrier.image = image;
		return *this;
	}


	VkImageMemoryBarrier vkBarrier;
};