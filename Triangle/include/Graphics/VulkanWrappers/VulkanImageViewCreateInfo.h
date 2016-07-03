#pragma once
#include <vulkan\vulkan.h>

class VulkanImageViewCreateInfo
{
public:
	VulkanImageViewCreateInfo()
	{
		vkInfo = {};
		vkInfo.pNext = NULL;
		vkInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	}

	VulkanImageViewCreateInfo& setViewType(VkImageViewType type)
	{
		vkInfo.viewType = type;
		return *this;
	}

	VulkanImageViewCreateInfo& setFlags(VkImageCreateFlags flags)
	{
		vkInfo.flags = flags;
		return *this;
	}

	VulkanImageViewCreateInfo& setImage(VkImage image)
	{
		vkInfo.image = image;
		return *this;
	}

	VulkanImageViewCreateInfo& setFormat(VkFormat format)
	{
		vkInfo.format = format;
		return *this;
	}

	VulkanImageViewCreateInfo& setComponents(VkComponentMapping components)
	{
		vkInfo.components = components;
		return *this;
	}

	VulkanImageViewCreateInfo& setSubresourceRange(VkImageSubresourceRange range)
	{
		vkInfo.subresourceRange = range;
		return *this;
	}

	VkImageViewCreateInfo vkInfo;
};