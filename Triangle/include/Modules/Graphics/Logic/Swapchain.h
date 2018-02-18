#pragma once
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"

typedef struct _SwapChainBuffers
{
	vk::Image image;
	vk::ImageView view;
} SwapChainBuffer;

class VulkanSwapChain
{
	const graphics::VulkanContext& context;
	vk::SurfaceKHR surface;

public:
	vk::Format colorFormat;
	vk::ColorSpaceKHR colorSpace;
	vk::SwapchainKHR swapChain = nullptr;
	vk::Extent2D swapchainExtent;

	std::vector<vk::Image> images;
	std::vector<SwapChainBuffer> buffers;

	// Index of the detected graphics and presenting device queue
	uint32_t queueNodeIndex = UINT32_MAX;

	VulkanSwapChain(const graphics::VulkanContext& context) : context(context)
	{}

	void initSurface(HINSTANCE platformHandle, HWND platformWindow, std::string name)
	{
		vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
			.setHinstance(platformHandle)
			.setHwnd(platformWindow);
		surface = context.instance.createWin32SurfaceKHR(surfaceCreateInfo);

		// Get available queue family properties
		std::vector<vk::QueueFamilyProperties> queueProps = context.physicalDevice.getQueueFamilyProperties();

		// Iterate over each queue to learn whether it supports presenting:
		// Find a queue with present support
		// Will be used to present the swap chain images to the windowing system
		std::vector<vk::Bool32> supportsPresent(queueProps.size());
		for(uint32_t i = 0; i < queueProps.size(); i++)
			supportsPresent.at(i) = context.physicalDevice.getSurfaceSupportKHR(i, surface);

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for(uint32_t i = 0; i < queueProps.size(); i++)
		{
			if(queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics)
			{
				if(graphicsQueueNodeIndex == UINT32_MAX)
				{
					graphicsQueueNodeIndex = i;
				}

				if(supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		if(presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for(uint32_t i = 0; i < queueProps.size(); ++i)
			{
				if(supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		// Exit if either a graphics or a presenting queue hasn't been found
		if(graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
		{
			std::cout << "Could not find a graphics and/or presenting queue! Fatal error" << std::endl;
			exit(1);
		}

		// todo : Add support for separate graphics and presenting queue
		if(graphicsQueueNodeIndex != presentQueueNodeIndex)
		{
			std::cout << "Separate graphics and presenting queues are not supported yet! Fatal error" << std::endl;
			exit(1);
		}

		queueNodeIndex = graphicsQueueNodeIndex;

		// Get list of supported surface formats
		std::vector<vk::SurfaceFormatKHR> surfaceFormats = context.physicalDevice.getSurfaceFormatsKHR(surface);

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if((surfaceFormats.size() == 1) && (surfaceFormats[0].format == vk::Format::eUndefined))
		{
			colorFormat = vk::Format::eB8G8R8A8Unorm;
		}
		else
		{
			// Always select the first available color format
			// If you need a specific format (e.g. SRGB) you'd need to
			// iterate over the list of available surface format and
			// check for it's presence
			colorFormat = surfaceFormats[0].format;
		}
		colorSpace = surfaceFormats[0].colorSpace;
	}

	void setup(vk::CommandBuffer cmdBuffer, uint32_t *width, uint32_t *height)
	{
		VkSwapchainKHR oldSwapchain = swapChain;

		// Get physical device surface properties and formats
		vk::SurfaceCapabilitiesKHR surfCaps = context.physicalDevice.getSurfaceCapabilitiesKHR(surface);

		// Get available present modes
		std::vector<vk::PresentModeKHR> presentModes = context.physicalDevice.getSurfacePresentModesKHR(surface);


		swapchainExtent = {};
		// width and height are either both -1, or both not -1.
		if(surfCaps.currentExtent.width == -1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = *width;
			swapchainExtent.height = *height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
			*width = surfCaps.currentExtent.width;
			*height = surfCaps.currentExtent.height;
		}

		// Prefer mailbox mode if present, it's the lowest latency non-tearing present  mode
		vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;
		for(size_t i = 0; i < presentModes.size(); i++)
		{
			if(presentModes[i] == vk::PresentModeKHR::eMailbox)
			{
				swapchainPresentMode = vk::PresentModeKHR::eMailbox;
				break;
			}
			if((swapchainPresentMode != vk::PresentModeKHR::eMailbox) 
				&& (presentModes[i] == vk::PresentModeKHR::eImmediate))
			{
				swapchainPresentMode = vk::PresentModeKHR::eImmediate;
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		vk::SurfaceTransformFlagBitsKHR preTransform;
		if(surfCaps.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		{
			preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		auto imageUsage = 
			vk::ImageUsageFlags(vk::ImageUsageFlagBits::eColorAttachment) | vk::ImageUsageFlagBits::eTransferDst;

		vk::SwapchainCreateInfoKHR swapchainCI = vk::SwapchainCreateInfoKHR()
			.setSurface(surface)
			.setMinImageCount(desiredNumberOfSwapchainImages)
			.setImageFormat(colorFormat)
			.setImageColorSpace(colorSpace)
			.setImageExtent(swapchainExtent)
			.setImageUsage(imageUsage)
			.setPreTransform(preTransform)
			.setImageArrayLayers(1)
			.setImageSharingMode(vk::SharingMode::eExclusive)
			.setPresentMode(swapchainPresentMode)
			.setOldSwapchain(oldSwapchain)
			.setClipped(true)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

		swapChain = context.device.createSwapchainKHR(swapchainCI);

		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if(oldSwapchain)
		{
			context.device.destroySwapchainKHR(oldSwapchain);
		}

		images = context.device.getSwapchainImagesKHR(swapChain);

		// Get the swap chain buffers containing the image and imageview
		buffers.resize(images.size());
		for(uint32_t i = 0; i < images.size(); i++)
		{
			vk::ImageViewCreateInfo colorAttachmentView;
			colorAttachmentView
				.setFormat(colorFormat)
				.setComponents(vk::ComponentMapping(
					vk::ComponentSwizzle::eR,
					vk::ComponentSwizzle::eG,
					vk::ComponentSwizzle::eB,
					vk::ComponentSwizzle::eA
				))
				.setViewType(vk::ImageViewType::e2D)
				.subresourceRange
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setLevelCount(1)
					.setLayerCount(1);
				
			buffers[i].image = images[i];

			// Create an image barrier object
			vk::ImageMemoryBarrier imageMemoryBarrier;

			// Some default values
			imageMemoryBarrier
				.setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
				.setOldLayout(vk::ImageLayout::eUndefined)
				.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
				.setImage(buffers[i].image)
				.subresourceRange
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setLevelCount(1)
				.setLayerCount(1);;

			// Put barrier on top
			vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
			vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

			// Put barrier inside setup command buffer
			cmdBuffer.pipelineBarrier(srcStageFlags, destStageFlags, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);

			///////////////////////////////////////////////////

			colorAttachmentView.image = buffers[i].image;

			buffers[i].view = context.device.createImageView(colorAttachmentView);
		}
	}

	// Acquires the next image in the swap chain
	void acquireNextImage(VkSemaphore presentCompleteSemaphore, VkFence fence, uint32_t *currentBuffer)
	{
		context.device.acquireNextImageKHR(swapChain, UINT64_MAX, presentCompleteSemaphore, fence, currentBuffer);
	}

	// Present the current image to the queue
	void queuePresent(vk::Queue queue, uint32_t* currentBuffer)
	{
		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.setSwapchainCount(1)
			.setPSwapchains(&swapChain)
			.setPImageIndices(currentBuffer);
		queue.presentKHR(presentInfo);
	}

	// Present the current image to the queue
	void queuePresent(vk::Queue queue, uint32_t* currentBuffer, vk::Semaphore waitSemaphore)
	{
		vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
			.setSwapchainCount(1)
			.setPSwapchains(&swapChain)
			.setPImageIndices(currentBuffer);

		if (waitSemaphore)
		{
		/*	presentInfo
				.setPWaitSemaphores(&waitSemaphore)
				.setWaitSemaphoreCount(1);*/
		}

		queue.presentKHR(&presentInfo);
	}

	void cleanup()
	{
		for (uint32_t i = 0; i < images.size(); i++)
		{
			context.device.destroyImageView(buffers[i].view);
		}
		context.device.destroySwapchainKHR(swapChain);
		context.instance.destroySurfaceKHR(surface);
	}

};