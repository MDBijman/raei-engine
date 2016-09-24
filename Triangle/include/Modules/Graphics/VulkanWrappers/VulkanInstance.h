#pragma once
#include "VulkanValidation.h"
#include "VulkanApplicationInfo.h"
#include "VulkanInstanceCreateInfo.h"
#include "VulkanPhysicalDevice.h"

#include <memory>
#include <vector>
#include <assert.h>
#include <iostream>
#include <vulkan/vulkan.h>

class VulkanInstance
{
public:
	explicit VulkanInstance(std::string name) 
	{
		// Set some info about our application
		VulkanApplicationInfo applicationInfo;
		applicationInfo
			.setApplicationName(name)
			.setEngineName(name)
			.setApiVersion(VK_MAKE_VERSION(1, 0, 13));

		// Extensions we want
		std::vector<const char*> extensions = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		};


		// Layers we want
		std::vector<const char*> layers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		// Set some info about the instance to be created
		VulkanInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo
			.setApplicationInfo(applicationInfo.vkInfo)
			.setExtensionCount(static_cast<uint32_t>(extensions.size()))
			.setExtensions(extensions.data())
			.setLayerCount(static_cast<uint32_t>(layers.size()))
			.setLayers(layers.data());

		VkResult err = vkCreateInstance(&instanceCreateInfo.vkInfo, nullptr, &vkInstance);
		assert(!err);

		{
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> layerPropertyList(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, layerPropertyList.data());
			std::cout << "Instance Layers: \n";
			for (auto &i : layerPropertyList)
			{
				std::cout << "  " << i.layerName << "\t\t | " << i.description << std::endl;
			}
			std::cout << std::endl;
		}

		ValidationFunctions functions(vkInstance);
		/* Setup callback creation information */
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		callbackCreateInfo.pNext = nullptr;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = &Validation::ValidationDebugCallback;
		callbackCreateInfo.pUserData = nullptr;

		/* Register the callback */
		VkDebugReportCallbackEXT callback;
		err = functions.fpCreateDebugReportCallbackEXT(vkInstance, &callbackCreateInfo, nullptr, &callback);
		assert(!err);
	}

	std::unique_ptr<std::vector<VulkanPhysicalDevice>> getPhysicalDevices() const
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