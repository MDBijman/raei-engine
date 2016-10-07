#pragma once
#include "VulkanValidation.h"

#include <vector>
#include <iostream>
#include <vulkan/vulkan.h>

class VulkanInstance
{
public:
	explicit VulkanInstance(std::string name)
	{
		uint32_t layerCount = 0;
		std::vector<vk::LayerProperties> layerPropertyList = vk::enumerateInstanceLayerProperties();
		std::cout << "Instance Layers: \n";
		for(auto &i : layerPropertyList)
		{
			std::cout << "  " << i.layerName << "\t\t | " << i.description << std::endl;
		}
		std::cout << std::endl;

		// Set some info about our application
		/*VulkanApplicationInfo applicationInfo;
		applicationInfo
			.setApplicationName(name)
			.setEngineName(name)
			.setEngineVersion(1)
			.setApiVersion(VK_MAKE_VERSION(1, 0, 21));
*/

// Extensions we want
		std::vector<const char*> extensions = {
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
			"VK_EXT_debug_report"
		};

		// Layers we want
		std::vector<const char*> layers = {
			"VK_LAYER_LUNARG_standard_validation",
		};

		// Set some info about the instance to be created
/*		VulkanInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo
			.setApplicationInfo(applicationInfo.vkInfo)
			.setExtensionCount(static_cast<uint32_t>(extensions.size()))
			.setExtensions(extensions.data())
			.setLayerCount(static_cast<uint32_t>(layers.size()))
			.setLayers(layers.data());*/

		vk::ApplicationInfo applicationInfo = vk::ApplicationInfo()
			.setPApplicationName(name.c_str())
			.setPEngineName(name.c_str())
			.setEngineVersion(1);

		vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
			.setPApplicationInfo(applicationInfo)
			.setEnabledExtensionCount(extensions.size())
			.setPpEnabledExtensionNames(extensions.data())
			.setEnabledLayerCount(layers.size())
			.setPpEnabledLayerNames(layers.data());

		instance = vk::createInstance(instanceCreateInfo);

		vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo = vk::DebugReportCallbackCreateInfoEXT()
			.setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning)
			.setPfnCallback(Validation::messageCallback());

		vk::createDebugReportCallbackEXT(callbackCreateInfo);
	}

	std::vector<vk::PhysicalDevice> getPhysicalDevices() const
	{
		return instance.enumeratePhysicalDevices();
	}

	vk::Instance instance;


};