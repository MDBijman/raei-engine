#pragma once
#include <vulkan\vulkan.h>

class VulkanBufferCreateInfo
{
public:
	VulkanBufferCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanBufferCreateInfo& setSize(uint32_t size)
	{
		vkInfo.size = size;
		return *this;
	}

	VulkanBufferCreateInfo& setUsage(VkBufferUsageFlagBits bits)
	{
		vkInfo.usage = bits;
		return *this;
	}

	VulkanBufferCreateInfo& setFlags(VkBufferCreateFlags flags)
	{
		vkInfo.flags = flags;
		return *this;
	}

	VkBufferCreateInfo vkInfo;
};