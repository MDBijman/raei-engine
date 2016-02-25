#pragma once
#include <vulkan/vulkan.h>

// Macro to get a procedure address based on a vulkan device
#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
{                                                                       \
    fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);   \
    if (fp##entrypoint == NULL)                                         \
	{																    \
        exit(1);                                                        \
    }                                                                   \
}

class SwapchainExtensions
{
public:
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;

	SwapchainExtensions(VkDevice device)
	{
		GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
		GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
		GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
		GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
		GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);
	}
};

