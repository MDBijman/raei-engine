#pragma once
#include <string>
#include <vulkan\vulkan.h>

class VulkanApplicationInfo
{
public:
	VulkanApplicationInfo()
	{
		vkInfo = {};
		vkInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	}

	VulkanApplicationInfo& setApplicationName(std::string name)
	{
		vkInfo.pApplicationName = name.c_str();
		return *this;
	}

	VulkanApplicationInfo& setEngineName(std::string name)
	{
		vkInfo.pEngineName = name.c_str();
		return *this;
	}	
	
	VkApplicationInfo vkInfo;
};
