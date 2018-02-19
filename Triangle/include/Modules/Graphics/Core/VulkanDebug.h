#pragma once
#include <vulkan/vulkan.h>
namespace vk { class Instance; }

VkBool32 messageCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t srcObject,
	size_t location,
	int32_t msgCode,
	const char* pLayerPrefix,
	const char* pMsg,
	void* pUserData);

// TODO add cleanup
class VulkanDebug
{
public:
	VulkanDebug();

	void initDebugging(vk::Instance& instance);

	PFN_vkCreateDebugReportCallbackEXT createDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDebugReportMessageEXT dbgBreakCallback = VK_NULL_HANDLE;

	VkDebugReportCallbackEXT msgCallback = VK_NULL_HANDLE;
};