#pragma once
#include <assert.h>
#include <vector>
#include <vulkan\vulkan.h>

// TODO initialize memory, queues etc. on create? 
class VulkanPhysicalDevice 
{
public:
	VulkanPhysicalDevice(VkPhysicalDevice d) : device(d) {}

	
	std::vector<VkQueueFamilyProperties>& queueFamilyProperties()
	{
		if (queueProperties)
			return *queueProperties;

		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, NULL);
		assert(queueCount >= 1);

		queueProperties = new std::vector<VkQueueFamilyProperties>();
		queueProperties->resize(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueProperties->data());
		return *queueProperties;
	}

	VkPhysicalDeviceMemoryProperties& memoryProperties()
	{
		if (memProperties)
			return *memProperties;

		vkGetPhysicalDeviceMemoryProperties(device, memProperties);
		return *memProperties;
	}

	VkFormatProperties& formatProperties(VkFormat format)
	{
		if (formProperties)
			return *formProperties;

		vkGetPhysicalDeviceFormatProperties(device, format, formProperties);
		return *formProperties;
	}

	operator VkPhysicalDevice() const { return device; }
	VkPhysicalDevice& getVkPhysicalDevice()
	{
		return device;
	}
private:
	VkPhysicalDevice device;

	VkPhysicalDeviceMemoryProperties* memProperties;
	VkFormatProperties* formProperties;
	std::vector<VkQueueFamilyProperties>* queueProperties;
};
