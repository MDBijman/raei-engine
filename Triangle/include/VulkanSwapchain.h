/*
* Class wrapping access to the swap chain
*
* Copyright (C) 2015 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <stdlib.h>
#include <string>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#endif

#include <vulkan/vulkan.h>
#include "SurfaceExtensions.h"
#include "SwapchainExtensions.h"


typedef struct _SwapChainBuffers {
	VkImage image;
	VkImageView view;
} SwapChainBuffer;

class VulkanSwapChain
{
private:
	VkInstance instance;
	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;
	// Function pointers
	SurfaceExtensions surfaceExt;
	SwapchainExtensions swapchainExt;

public:
	VkFormat colorFormat;
	VkColorSpaceKHR colorSpace;
	VkImage* swapchainImages;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;

	uint32_t imageCount;
	SwapChainBuffer* buffers;

	// Index of the deteced graphics and presenting device queue
	uint32_t queueNodeIndex = UINT32_MAX;

	VulkanSwapChain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device) : surfaceExt(instance), swapchainExt(device)
	{
		this->instance = instance;
		this->physicalDevice = physicalDevice;
		this->device = device;
	}

	// wip naming
	void initSwapChain(HINSTANCE platformHandle, HWND platformWindow, std::string name)
	{
		uint32_t queueCount;
		VkQueueFamilyProperties *queueProps;

		VkResult err;

		// Create surface depending on OS
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hinstance = (HINSTANCE)platformHandle;
		surfaceCreateInfo.hwnd = (HWND)platformWindow;
		err = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);

		uint32_t i;

		// Get queue properties
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

		queueProps = (VkQueueFamilyProperties *)malloc(queueCount * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps);
		assert(queueCount >= 1);

		// Iterate over each queue to learn whether it supports presenting:
		VkBool32* supportsPresent = (VkBool32 *)malloc(queueCount * sizeof(VkBool32));
		for (i = 0; i < queueCount; i++)
		{
			surfaceExt.fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i,
				surface,
				&supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (i = 0; i < queueCount; i++)
		{
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
				{
					graphicsQueueNodeIndex = i;
				}
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
				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		free(supportsPresent);

		queueNodeIndex = graphicsQueueNodeIndex;

		// Get list of supported formats
		uint32_t formatCount;
		err = surfaceExt.fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
		assert(!err);

		VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
		err = surfaceExt.fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfFormats);
		assert(!err);

		// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
		// the surface has no preferred format.  Otherwise, at least one
		// supported format will be returned.
		if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		}
		else
		{
			assert(formatCount >= 1);
			colorFormat = surfFormats[0].format;
		}
		colorSpace = surfFormats[0].colorSpace;
	}

	

	void setup(VkCommandBuffer cmdBuffer, uint32_t *width, uint32_t *height)
	{
		VkResult err;
		VkSwapchainKHR oldSwapchain = swapChain;

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		err = surfaceExt.fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
		assert(!err);

		uint32_t presentModeCount;
		err = surfaceExt.fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
		assert(!err);

		std::vector<VkPresentModeKHR> presentModes;
		presentModes.resize(presentModeCount);

		err = surfaceExt.fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
		assert(!err);

		VkExtent2D swapchainExtent = {};
		// width and height are either both -1, or both not -1.
		if (surfaceCapabilities.currentExtent.width == -1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfaceCapabilities.currentExtent;
			*width = surfaceCapabilities.currentExtent.width;
			*height = surfaceCapabilities.currentExtent.height;
		}

		// Try to use mailbox mode
		// Low latency and non-tearing
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount + 1;
		if ((surfaceCapabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;
		}

		VkSurfaceTransformFlagsKHR preTransform;
		if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else {
			preTransform = surfaceCapabilities.currentTransform;
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCI.pNext = NULL;
		swapchainCI.surface = surface;
		swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCI.imageFormat = colorFormat;
		swapchainCI.imageColorSpace = colorSpace;
		swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCI.imageArrayLayers = 1;
		swapchainCI.queueFamilyIndexCount = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCI.queueFamilyIndexCount = 0;
		swapchainCI.pQueueFamilyIndices = NULL;
		swapchainCI.presentMode = swapchainPresentMode;
		swapchainCI.oldSwapchain = oldSwapchain;
		swapchainCI.clipped = true;
		swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		err = swapchainExt.fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain);
		assert(!err);

		// If we just re-created an existing swapchain, we should destroy the old
		// swapchain at this point.
		// Note: destroying the swapchain also cleans up all its associated
		// presentable images once the platform is done with them.
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			swapchainExt.fpDestroySwapchainKHR(device, oldSwapchain, nullptr);
		}

		err = swapchainExt.fpGetSwapchainImagesKHR(device, swapChain, &imageCount, NULL);
		assert(!err);

		swapchainImages = (VkImage*)malloc(imageCount * sizeof(VkImage));
		assert(swapchainImages);
		err = swapchainExt.fpGetSwapchainImagesKHR(device, swapChain, &imageCount, swapchainImages);
		assert(!err);

		buffers = (SwapChainBuffer*)malloc(sizeof(SwapChainBuffer)*imageCount);
		assert(buffers);
		//buffers.resize(imageCount);
		for (uint32_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = NULL;
			colorAttachmentView.format = colorFormat;
			colorAttachmentView.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			buffers[i].image = swapchainImages[i];

			// Create an image barrier object
			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.pNext = NULL;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			imageMemoryBarrier.image = buffers[i].image;
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
			imageMemoryBarrier.subresourceRange.levelCount = 1;
			imageMemoryBarrier.subresourceRange.layerCount = 1;

			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;

			// Put barrier on top
			VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(cmdBuffer, srcStageFlags, destStageFlags, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

			colorAttachmentView.image = buffers[i].image;

			err = vkCreateImageView(device, &colorAttachmentView, nullptr, &buffers[i].view);
			assert(!err);
		}
	}

	// Acquires the next image in the swap chain
	void acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t *currentBuffer)
	{
		VkResult error = swapchainExt.fpAcquireNextImageKHR(device, swapChain, UINT64_MAX, presentCompleteSemaphore, (VkFence)nullptr, currentBuffer);
		assert(!error);
	}

	// Present the current image to the queue
	void queuePresent(VkQueue queue, uint32_t currentBuffer)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapChain;
		presentInfo.pImageIndices = &currentBuffer;
		VkResult error = swapchainExt.fpQueuePresentKHR(queue, &presentInfo);
		assert(!error);
	}

	void cleanup()
	{
		for (uint32_t i = 0; i < imageCount; i++)
		{
			vkDestroyImageView(device, buffers[i].view, nullptr);
		}
		swapchainExt.fpDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}

};
