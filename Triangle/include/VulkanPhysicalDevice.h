#pragma once
#include <assert.h>
#include <vector>
#include <vulkan\vulkan.h>

class VulkanPhysicalDevice
{
public:
	VulkanPhysicalDevice(VkPhysicalDevice d) : device(d) {}


	std::vector<VkQueueFamilyProperties> queueFamilyProperties()
	{
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, NULL);
		assert(queueCount >= 1);

		std::vector<VkQueueFamilyProperties> queueProperties;
		queueProperties.resize(queueCount);

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueProperties.data());
		return queueProperties;
	}

	VkPhysicalDeviceMemoryProperties memoryProperties()
	{
		VkPhysicalDeviceMemoryProperties props;
		vkGetPhysicalDeviceMemoryProperties(device, &props);
		return props;
	}

	VkFormatProperties formatProperties(VkFormat format)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(device, format, &props);
		return props;
	}

	operator VkPhysicalDevice() const { return device; }

private:
	VkPhysicalDevice device;

};
