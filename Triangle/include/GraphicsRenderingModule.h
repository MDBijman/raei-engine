#pragma once
#include <math.h>
#include <vector>
#include <chrono>
#include <memory>

#include "VulkanWrappers.h"
#include "Shaders.h"
#include "Drawable.h"

class GraphicsRenderingModule 
{
public:
	GraphicsRenderingModule(VulkanDevice& device, VkCommandPool& cmdPool, VulkanSwapChain& swapchain, VulkanQueue& queue);
	
	void prepare();
	void submit(Drawable& c);
	void present();

private:
	void submitPrePresentBarrier(VkImage image);
	void submitPostPresentBarrier(VkImage image);

	uint32_t currentBuffer = 0;

	VulkanDevice&        device;
	VulkanSwapChain&     swapchain;
	VulkanQueue&         queue;

	VkSemaphore          presentComplete;
	VkSemaphore          renderComplete;

	VulkanCommandBuffer  postPresentCmdBuffer;
	VulkanCommandBuffer  prePresentCmdBuffer;
};