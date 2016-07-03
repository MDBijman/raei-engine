#pragma once
#include "Graphics\VulkanWrappers\VulkanWrappers.h"

#include <vulkan\vulkan.h>
#include <assert.h>
#include <gli\gli.hpp>

class Texture
{
public:
	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkDeviceMemory deviceMemory;
	VkImageView view;
	uint32_t width, height;
	uint32_t mipLevels;

	// Get appropriate memory type index for a memory allocation
	uint32_t getMemoryType(uint32_t typeBits, VkFlags properties, VkPhysicalDeviceMemoryProperties deviceMemoryProperties);

	void load(const char* filename, VkFormat format, VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, VkCommandPool& pool, VulkanQueue& queue);
};