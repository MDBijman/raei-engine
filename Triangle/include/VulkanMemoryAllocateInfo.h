#pragma once
#include <vulkan\vulkan.h>

class VulkanMemoryAllocateInfo
{
public:
	VulkanMemoryAllocateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanMemoryAllocateInfo& setAllocationSize(VkDeviceSize size)
	{
		vkInfo.allocationSize = size;
		return *this;
	}

	VulkanMemoryAllocateInfo& setMemoryTypeIndex(uint32_t type)
	{
		vkInfo.memoryTypeIndex = type;
		return *this;
	}

	VkMemoryAllocateInfo vkInfo;
};