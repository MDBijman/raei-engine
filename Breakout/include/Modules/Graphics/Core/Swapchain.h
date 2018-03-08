#pragma once
#include "Modules/Graphics/Core/VulkanContext.h"

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

	VulkanSwapChain(const graphics::VulkanContext& context);

	void initSurface(HINSTANCE platformHandle, HWND platformWindow, std::string name);

	void setup(vk::CommandBuffer cmdBuffer, uint32_t *width, uint32_t *height);

	// Acquires the next image in the swap chain
	void acquireNextImage(VkSemaphore presentCompleteSemaphore, VkFence fence, uint32_t *currentBuffer);

	// Present the current image to the queue
	void queuePresent(vk::Queue queue, uint32_t* currentBuffer);

	// Present the current image to the queue
	// TODO remove semaphore from signature
	void queuePresent(vk::Queue queue, uint32_t* currentBuffer, vk::Semaphore waitSemaphore);

	void cleanup();
};