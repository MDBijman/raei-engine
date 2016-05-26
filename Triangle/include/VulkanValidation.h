#pragma once
#include <vulkan\vulkan.h>
#include <iostream>

namespace Validation {
	static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationDebugCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char*                 pLayerPrefix,
		const char*                 pMessage,
		void*                       pUserData)
	{
		std::cout << pLayerPrefix << ": " << pMessage << std::endl;
		return VK_FALSE;
	}
}

// Macro to get a procedure address based on a vulkan instance
#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
{                                                                       \
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(inst, "vk"#entrypoint); \
    if (fp##entrypoint == NULL)                                         \
	{																    \
        std::cout << "err" << std::endl;                                                        \
    }                                                                   \
}

class ValidationFunctions 
{
public:
	PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT fpDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT;

	ValidationFunctions(VkInstance instance)
	{
		GET_INSTANCE_PROC_ADDR(instance, CreateDebugReportCallbackEXT);
		GET_INSTANCE_PROC_ADDR(instance, DebugReportMessageEXT);
		GET_INSTANCE_PROC_ADDR(instance, DestroyDebugReportCallbackEXT);
	}
};

