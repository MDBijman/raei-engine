#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include <iterator>
#include <vulkan\vulkan.h>

#include "VulkanPhysicalDevice.h"

class VulkanInstance
{
public:
	VulkanInstance(std::string name)
	{
		// Set some info about our application
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = name.c_str();
		appInfo.pEngineName = name.c_str();

		// Extensions we want
		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
		};

		// Set some info about the instance to be created
		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		VkResult err = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
		assert(!err);
	}

	std::vector<VulkanPhysicalDevice>& getPhysicalDevices()
	{
		if (physicalDevices)
			return *physicalDevices;

		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

		std::vector<VkPhysicalDevice> vkPhysicalDevices;
		vkPhysicalDevices.resize(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, vkPhysicalDevices.data());

		physicalDevices = new std::vector<VulkanPhysicalDevice>();
		physicalDevices->resize(deviceCount);
		std::transform(vkPhysicalDevices.begin(), vkPhysicalDevices.end(), std::back_inserter(physicalDevices), [](VkPhysicalDevice d) -> VulkanPhysicalDevice { return VulkanPhysicalDevice(d); });
		return *physicalDevices;
	}

	operator VkInstance() const { return instance; }
	VkInstance& getVkInstance()
	{
		return instance;
	}
private:
	VkInstance instance;

	std::vector<VulkanPhysicalDevice>* physicalDevices;

};