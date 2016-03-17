#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include <iterator>
#include <vulkan\vulkan.h>

#include "VulkanPhysicalDevice.h"
#include "VulkanInstanceCreateInfo.h"
#include "VulkanApplicationInfo.h"

class VulkanInstance
{
public:
	VulkanInstance(std::string name) 
	{
		// Set some info about our application
		VulkanApplicationInfo applicationInfo;
		applicationInfo
			.setApplicationName(name)
			.setEngineName(name);

		// Extensions we want
		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};

		// Layers we want
		std::vector<const char*> layers = {
		};

		// Set some info about the instance to be created
		VulkanInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo
			.setApplicationInfo(applicationInfo.vkInfo)
			.setExtensionCount(extensions.size())
			.setExtensions(extensions.data())
			.setLayerCount(layers.size())
			.setLayers(layers.data());

		VkResult err = vkCreateInstance(&instanceCreateInfo.vkInfo, nullptr, &vkInstance);
		assert(!err);
	}

	std::unique_ptr<std::vector<VulkanPhysicalDevice>> getPhysicalDevices()
	{
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, NULL);

		std::vector<VkPhysicalDevice> vkPhysicalDevices;
		vkPhysicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, vkPhysicalDevices.data());

		// Create wrapped objects
		std::unique_ptr<std::vector<VulkanPhysicalDevice>> physicalDevices = std::make_unique<std::vector<VulkanPhysicalDevice>>();
		for (VkPhysicalDevice pd : vkPhysicalDevices) 
			physicalDevices->push_back(VulkanPhysicalDevice(pd));

		return physicalDevices;
	}

	VkInstance vkInstance;


};