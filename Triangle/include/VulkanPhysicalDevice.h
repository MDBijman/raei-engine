#pragma once
#include <assert.h>
#include <vector>
#include <vulkan\vulkan.h>

class VulkanPhysicalDevice 
{
public:
	VulkanPhysicalDevice(VkPhysicalDevice d) : device(d) {}

	
	std::unique_ptr<std::vector<VkQueueFamilyProperties>> queueFamilyProperties()
	{
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, NULL);
		assert(queueCount >= 1);

		std::unique_ptr<std::vector<VkQueueFamilyProperties>> queueProperties = std::make_unique<std::vector<VkQueueFamilyProperties>>();
		queueProperties->resize(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueProperties->data());

		return queueProperties;
	}

	VkPhysicalDeviceMemoryProperties& memoryProperties()
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
		return memProperties;
	}

	VkFormatProperties& formatProperties(VkFormat format)
	{
		VkFormatProperties formProperties;
		vkGetPhysicalDeviceFormatProperties(device, format, &formProperties);
		return formProperties;
	}

	operator VkPhysicalDevice() const { return device; }
	VkPhysicalDevice& getVkPhysicalDevice()
	{
		return device;
	}
private:
	VkPhysicalDevice device;

};
