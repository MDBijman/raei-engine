#pragma once
#include <assert.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <memory>

class VulkanPhysicalDevice 
{
public:
	explicit VulkanPhysicalDevice(VkPhysicalDevice d) : vkPhysicalDevice(d) {}

	std::unique_ptr<std::vector<VkQueueFamilyProperties>> queueFamilyProperties() const
	{
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueCount, NULL);
		assert(queueCount >= 1);

		std::unique_ptr<std::vector<VkQueueFamilyProperties>> queueProperties = std::make_unique<std::vector<VkQueueFamilyProperties>>();
		queueProperties->resize(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueCount, queueProperties->data());

		return queueProperties;
	}

	uint32_t getMemoryPropertyIndex(VkMemoryPropertyFlagBits flag, VkMemoryRequirements requirements) const
	{
		for (uint32_t i = 0; i < 32; i++)
		{
			if ((requirements.memoryTypeBits & 1) == 1)
			{
				if ((memoryProperties().memoryTypes[i].propertyFlags & flag) == flag)
				{
					return i;
				}
			}
			requirements.memoryTypeBits >>= 1;
		}
		return -1;
	}

	VkPhysicalDeviceMemoryProperties memoryProperties() const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);
		return memProperties;
	}

	VkFormatProperties formatProperties(VkFormat format) const
	{
		VkFormatProperties formProperties;
		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &formProperties);
		return formProperties;
	}

	VkPhysicalDevice vkPhysicalDevice;
};
