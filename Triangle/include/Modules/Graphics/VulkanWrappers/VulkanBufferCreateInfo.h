#pragma once
#include <vulkan\vulkan.h>

class VulkanBufferCreateInfo
{
public:
	VulkanBufferCreateInfo()
	{
		vk = {};
		vk.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk.pNext = NULL;
	}

	VulkanBufferCreateInfo& setSize(uint32_t size)
	{
		vk.size = size;
		return *this;
	}

	VulkanBufferCreateInfo& setUsage(VkBufferUsageFlagBits bits)
	{
		vk.usage = bits;
		return *this;
	}

	VulkanBufferCreateInfo& setFlags(VkBufferCreateFlags flags)
	{
		vk.flags = flags;
		return *this;
	}

	VkBufferCreateInfo vk;
};