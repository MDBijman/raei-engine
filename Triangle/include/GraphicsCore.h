#pragma once
#include <glm\glm.hpp>
#include <memory>
#include <vector>

#include "VulkanWrappers.h"
#include "GraphicsRenderingModule.h"

class Drawable;

class GraphicsCore
{
public:
	struct
	{
		std::unique_ptr<GraphicsRenderingModule> renderer;
	} modules;

	std::unique_ptr<VulkanInstance>       instance;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::unique_ptr<VulkanDevice>         device; 
	std::unique_ptr<VulkanSwapChain>      swapchain;
	std::unique_ptr<VulkanQueue>          queue;

	VkFormat depthFormat;
	VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;

	VkCommandPool cmdPool;
	std::vector<VulkanCommandBuffer> drawCmdBuffers;
	VulkanCommandBuffer setupCmdBuffer = VK_NULL_HANDLE;

	struct
	{
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depthStencil;

	VkRenderPass renderPass;
	VkPipelineCache pipelineCache;
	std::vector<VkFramebuffer> frameBuffers;

	const glm::vec2 SCREEN_DIMENSIONS;

	/*
	* Initializes a unique Vulkan Instance for this application.
	* Also initializes physical devices, queue family properties, physical device memory properties, the swapchain, and the queue.
	*/
	GraphicsCore(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height);

	void render(std::vector<Drawable> d);

private:
	void prepareCommandPool();
	void prepareSetupCommandBuffer();
	void prepareDepthStencil(uint32_t width, uint32_t height);
	void prepareRenderPass();
	void preparePipelineCache();
	void prepareFramebuffers(uint32_t width, uint32_t height);

	
};