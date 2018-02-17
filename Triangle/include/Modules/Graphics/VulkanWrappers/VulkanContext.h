#pragma once
#include "VulkanDebug.h"
#include <vulkan/VULKAN.HPP>
#include <iostream>

namespace Graphics
{
	class VulkanContext
	{
	public:
		explicit VulkanContext(std::string name)
		{
			uint32_t layerCount = 0;
			std::vector<vk::LayerProperties> layerPropertyList = vk::enumerateInstanceLayerProperties();
			std::cout << "Instance Layers: \n";
			for(auto &i : layerPropertyList)
			{
				std::cout << "  " << i.layerName << "\t\t | " << i.description << std::endl;
			}
			std::cout << std::endl;

			// Extensions we want
			std::vector<const char*> extensions = {
				"VK_KHR_surface",
				"VK_KHR_win32_surface",
				"VK_EXT_debug_report"
			};

			// Layers we want
			std::vector<const char*> layers = {
				//"VK_LAYER_LUNARG_standard_validation",
			};

			vk::ApplicationInfo applicationInfo = vk::ApplicationInfo()
				.setPApplicationName(name.c_str())
				.setPEngineName(name.c_str())
				.setEngineVersion(1)
				.setApiVersion(VK_MAKE_VERSION(1, 0, 65));

			vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo()
				.setPApplicationInfo(&applicationInfo)
				.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
				.setPpEnabledExtensionNames(extensions.data())
				.setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
				.setPpEnabledLayerNames(layers.data());

			instance = vk::createInstance(instanceCreateInfo);
			debug.initDebugging(instance);

			physicalDevice = vk::PhysicalDevice(instance.enumeratePhysicalDevices().at(0));

			createDevice();
		}

		vk::MemoryAllocateInfo getMemoryRequirements(vk::Buffer buffer, vk::MemoryPropertyFlags flag)
		{
			// Get memory requirements including size, alignment and memory type 
			vk::MemoryRequirements memReqs = device.getBufferMemoryRequirements(buffer);

			uint32_t memoryTypeIndex = -1;

			auto properties = physicalDevice.getMemoryProperties();
			for(uint32_t i = 0; i < 32; i++)
			{
				if(memReqs.memoryTypeBits & 1)
				{
					if(properties.memoryTypes[i].propertyFlags & flag)
					{
						memoryTypeIndex = i;
						break;
					}
				}
				memReqs.memoryTypeBits >>= 1;
			}

			vk::MemoryAllocateInfo allocInfo;
			allocInfo
				.setAllocationSize(memReqs.size)
				.setMemoryTypeIndex(memoryTypeIndex);

			return allocInfo;
		}

		vk::Instance instance;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		uint32_t graphicsQueueIndex;

	private:
		VulkanDebug debug;

		void createDevice()
		{
			auto queueProperties = physicalDevice.getQueueFamilyProperties();

			for(graphicsQueueIndex = 0; graphicsQueueIndex < queueProperties.size(); graphicsQueueIndex++)
			{
				if(queueProperties.at(graphicsQueueIndex).queueFlags & vk::QueueFlagBits::eGraphics)
					break;
			}

			std::array<float, 1> queuePriorities = { 0.0f };
			vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(graphicsQueueIndex)
				.setQueueCount(1)
				.setPQueuePriorities(queuePriorities.data());

			std::vector<const char*> enabledExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};

			vk::PhysicalDeviceFeatures features;
			features
				.setTextureCompressionBC(true)
				.setSamplerAnisotropy(true);

			vk::DeviceCreateInfo deviceCreateInfo = vk::DeviceCreateInfo()
				.setQueueCreateInfoCount(1)
				.setPQueueCreateInfos(&queueCreateInfo)
				.setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()))
				.setPpEnabledExtensionNames(enabledExtensions.data())
				.setPEnabledFeatures(&features);

			device = physicalDevice.createDevice(deviceCreateInfo);
		}
	};
}
