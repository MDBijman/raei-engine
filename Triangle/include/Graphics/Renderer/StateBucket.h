#pragma once
#include "Graphics\VulkanWrappers\VulkanWrappers.h"

#include <memory>

namespace Graphics
{
	class StateBucket
	{
	public:
		StateBucket();

		std::unique_ptr<VulkanInstance>       instance;
		std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
		std::unique_ptr<VulkanDevice>         device;
		std::unique_ptr<VulkanSwapChain>      swapchain;
		std::unique_ptr<VulkanQueue>          queue;

		VkFormat depthFormat;
		VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;

		VkCommandPool              cmdPool;
		VkRenderPass               renderPass;
		VkPipelineCache            pipelineCache;
		std::vector<VkFramebuffer> frameBuffers;

		struct
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		} depthStencil;
	};
}
