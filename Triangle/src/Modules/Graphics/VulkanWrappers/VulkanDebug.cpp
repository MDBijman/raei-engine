#include "stdafx.h"
#include "Modules/Graphics/VulkanWrappers/VulkanDebug.h"
#include <vulkan/VULKAN.HPP>
#include <iostream>
#include <sstream>

VkBool32 messageCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t srcObject,
	size_t location,
	int32_t msgCode,
	const char* pLayerPrefix,
	const char* pMsg,
	void* pUserData)
{
	std::string message;
	{
		std::stringstream buf;

		if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			buf << "ERROR: ";
		}
		else if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			buf << "WARNING: ";
		}
		else if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			buf << "PERF: ";
		}
		buf << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;
		message = buf.str();
	}

	std::cout << message << std::endl;
	return VK_FALSE;
}

VulkanDebug::VulkanDebug()
{}

void VulkanDebug::initDebugging(vk::Instance& instance)
{
	createDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	destroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	dbgBreakCallback = (PFN_vkDebugReportMessageEXT) vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");

	VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
	dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgCreateInfo.pNext = nullptr;
	dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT) messageCallback;
	dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

	VkResult err = createDebugReportCallback(instance, &dbgCreateInfo, nullptr, &msgCallback);
}
