#pragma once
#include <vulkan\vulkan.h>

class VulkanInstanceCreateInfo
{
public:
	VulkanInstanceCreateInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vkInfo.pNext = NULL;
	}

	VulkanInstanceCreateInfo& setApplicationInfo(VkApplicationInfo appInfo)
	{
		vkInfo.pApplicationInfo = &appInfo;
		return *this;
	}

	VulkanInstanceCreateInfo& setExtensionCount(uint32_t count)
	{
		vkInfo.enabledExtensionCount = count;
		return *this;
	}

	VulkanInstanceCreateInfo& setExtensions(const char** names)
	{
		vkInfo.ppEnabledExtensionNames = names;
		return *this;
	}

	VulkanInstanceCreateInfo& setLayerCount(uint32_t count)
	{
		vkInfo.enabledLayerCount = count;
		return *this;
	}

	VulkanInstanceCreateInfo& setLayers(const char** names)
	{
		vkInfo.ppEnabledLayerNames = names;
		return *this;
	}

	VkInstanceCreateInfo vkInfo;
};
