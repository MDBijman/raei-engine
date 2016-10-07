#pragma once
#include <vulkan/vulkan.h>

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
		this->appName = name;
		vkInfo.pApplicationName = appName.c_str();
		return *this;
	}

	VulkanApplicationInfo& setEngineName(std::string name)
	{
		this->engineName = name;
		vkInfo.pEngineName = engineName.c_str();
		return *this;
	}	

	VulkanApplicationInfo& setApiVersion(uint32_t version)
	{
		vkInfo.apiVersion = version;
		return *this;
	}

	VulkanApplicationInfo& setEngineVersion(uint32_t version)
	{
		vkInfo.engineVersion = version;
		return *this;
	}
	
	VkApplicationInfo vkInfo;

private:
	std::string appName;
	std::string engineName;
};
