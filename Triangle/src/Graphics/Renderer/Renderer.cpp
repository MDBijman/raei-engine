#include "Graphics\Renderer\Renderer.h"

#include "Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Geometry\Vertex.h"

#include <vector>

namespace Graphics
{
	Renderer::Renderer(WindowsContext context)
	{
		instance = std::make_unique<VulkanInstance>("triangle");
		physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance->getPhysicalDevices()->at(0));

		auto queueProperties = physicalDevice->queueFamilyProperties();
		uint32_t graphicsQueueIndex;
		for (graphicsQueueIndex = 0; graphicsQueueIndex < queueProperties->size(); graphicsQueueIndex++)
		{
			if (queueProperties->at(graphicsQueueIndex).queueFlags & VK_QUEUE_GRAPHICS_BIT)
				break;
		}
		assert(graphicsQueueIndex < queueProperties->size());

		device = std::make_unique<VulkanDevice>(physicalDevice->vkPhysicalDevice, graphicsQueueIndex);
		swapchain = std::make_unique<VulkanSwapChain>(instance->vkInstance, physicalDevice->vkPhysicalDevice, device->vkDevice);
		queue = std::make_unique<VulkanQueue>(device->queueAt(graphicsQueueIndex));

		// Find supported depth format
		// We prefer 24 bits of depth and 8 bits of stencil, but that may not be supported by all implementations
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
		for (auto& format : depthFormats)
		{
			if (physicalDevice->formatProperties(format).optimalTilingFeatures && VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				depthFormat = format;
				break;
			}
		}

		swapchain->initSurface(context.hInstance, context.window, context.name);

		VulkanCommandPoolCreateInfo cmdPoolInfo;
		cmdPoolInfo
			.setQueueFamilyIndex(swapchain->queueNodeIndex)
			.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		cmdPool = device->createCommandPool(cmdPoolInfo.vkInfo);

		VulkanCommandBufferBeginInfo cmdBufferBeginInfo;
		VulkanCommandBuffer setupCmdBuffer;
		VulkanCommandBufferAllocateInfo info;
		info.setCommandPool(cmdPool)
			.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
			.setCommandBufferCount(1);

		setupCmdBuffer = device->allocateCommandBuffers(info.vkInfo).at(0);
		setupCmdBuffer.beginCommandBuffer(cmdBufferBeginInfo.vkInfo);
		swapchain->setup(setupCmdBuffer.vkBuffer, &context.width, &context.height);
		prepareDepthStencil(context.width, context.height);
		prepareRenderPass();
		preparePipelineCache();
		prepareFramebuffers(context.width, context.height);
		setupCmdBuffer.end();

		VulkanSubmitInfo submitInfo;
		std::vector<VkCommandBuffer> buffers{ setupCmdBuffer.vkBuffer };
		submitInfo.setCommandBuffers(buffers);
		queue->submit(1, submitInfo.vkInfo);
		queue->waitIdle();
		device->freeCommandBuffer(cmdPool, setupCmdBuffer);

		VulkanCommandBufferAllocateInfo info2;
		info2.setCommandPool(cmdPool)
			.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
			.setCommandBufferCount(1);

		postPresentCmdBuffer = device->allocateCommandBuffers(info2.vkInfo).at(0);
		prePresentCmdBuffer = device->allocateCommandBuffers(info2.vkInfo).at(0);

		VulkanSemaphoreCreateInfo semaphoreCreateInfo;

		// This semaphore ensures that the image is complete
		// before starting to submit again
		presentComplete = device->createSemaphore(semaphoreCreateInfo.vkInfo);

		// This semaphore ensures that all commands submitted
		// have been finished before submitting the image to the queue
		renderComplete = device->createSemaphore(semaphoreCreateInfo.vkInfo);
	}

	void Renderer::render(std::vector<Drawable> d)
	{
		prepare();
		for (auto it = d.begin(); it != d.end(); ++it)
			submit(*it);
		present();
	}

	void Renderer::prepare()
	{
		swapchain->acquireNextImage(presentComplete, &currentBuffer);
		submitPostPresentBarrier(swapchain->buffers[currentBuffer].image);
	}

	void Renderer::submit(Drawable& c)
	{
		// The submit info structure contains a list of
		// command buffers and semaphores to be submitted to a queue
		// If you want to submit multiple command buffers, pass an array
		VulkanSubmitInfo submitInfo;
		VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		std::vector<VkCommandBuffer> commandBuffers{ c.getCommandBuffers()[currentBuffer].vkBuffer };
		std::vector<VkSemaphore> wait{ presentComplete };
		std::vector<VkSemaphore> signal{ renderComplete };
		submitInfo.setCommandBuffers(commandBuffers)
			.setDstStageMask(&submitPipelineStages)
			.setSignalSemaphores(signal)
			.setWaitSemaphores(wait);

		// Submit to the graphics queue
		queue->submit(1, submitInfo.vkInfo);
	}

	void Renderer::present()
	{
		submitPrePresentBarrier(swapchain->buffers[currentBuffer].image);

		swapchain->queuePresent(*queue, currentBuffer, renderComplete);

		VkResult err = vkQueueWaitIdle(*queue);
		assert(!err);
	}

	void Renderer::submitPostPresentBarrier(VkImage image)
	{
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.pNext = NULL;

		VkResult err = (vkBeginCommandBuffer(postPresentCmdBuffer.vkBuffer, &cmdBufInfo));
		assert(!err);

		VkImageMemoryBarrier postPresentBarrier = {};
		postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		postPresentBarrier.pNext = NULL;
		postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.srcAccessMask = 0;
		postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		postPresentBarrier.image = image;

		vkCmdPipelineBarrier(
			postPresentCmdBuffer.vkBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr, // No memory barriers,
			0, nullptr, // No buffer barriers,
			1, &postPresentBarrier);

		err = (vkEndCommandBuffer(postPresentCmdBuffer.vkBuffer));
		assert(!err);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &postPresentCmdBuffer.vkBuffer;

		err = vkQueueSubmit(*queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!err);
	}

	void Renderer::prepareDepthStencil(uint32_t width, uint32_t height)
	{
		VulkanImageCreateInfo imageCreateInfo;
		imageCreateInfo
			.setImageType(VK_IMAGE_TYPE_2D)
			.setFormat(depthFormat)
			.setExtent({ width, height, 1 })
			.setMipLevels(1).setArrayLayers(1)
			.setSamples(VK_SAMPLE_COUNT_1_BIT)
			.setTiling(VK_IMAGE_TILING_OPTIMAL)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.setFlags(0);
		depthStencil.image = device->createImage(imageCreateInfo.vkInfo);

		VulkanImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo
			.setViewType(VK_IMAGE_VIEW_TYPE_2D)
			.setFormat(depthFormat)
			.setFlags(0);
		VulkanImageSubresourceRange subresourceRange;
		subresourceRange
			.setAspectMask(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);
		imageViewCreateInfo.setSubresourceRange(subresourceRange.vkRange);

		VkMemoryRequirements memReqs = device->getImageMemoryRequirements(depthStencil.image);

		VulkanMemoryAllocateInfo memoryInfo;
		memoryInfo.setAllocationSize(memReqs.size);
		memoryInfo.setMemoryTypeIndex(physicalDevice->getMemoryPropertyIndex(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memReqs));

		depthStencil.mem = device->allocateMemory(memoryInfo.vkInfo);

		device->bindImageMemory(depthStencil.image, depthStencil.mem);

		imageViewCreateInfo.setImage(depthStencil.image);
		depthStencil.view = device->createImageView(imageViewCreateInfo.vkInfo);
	}

	void Renderer::prepareRenderPass()
	{	
		// Attachments
		// Color attachment
		std::vector<VulkanAttachmentDescription> attachments(2);
		attachments[0]
			.setFormat(colorformat)
			.setSamples(VK_SAMPLE_COUNT_1_BIT)
			.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
			.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
			.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
			.setInitialLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.setFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		// Depth attachment
		attachments[1] 
			.setFormat(depthFormat)
			.setSamples(VK_SAMPLE_COUNT_1_BIT)
			.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
			.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
			.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
			.setInitialLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.setFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		// Subpasses
		std::vector<VulkanAttachmentReference> colorReference(1);
		// Color reference
		colorReference[0]
			.setAttachment(0)
			.setImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		// Depth reference
		VulkanAttachmentReference depthReference;
		depthReference
			.setAttachment(1)
			.setImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		std::vector<VulkanSubpassDescription> subpass(1);
		subpass[0]
			.setPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS)
			.setFlags(0)
			.setInputAttachments(std::vector<VulkanAttachmentReference>(0))
			.setColorAttachments(colorReference)
			.setResolveAttachments(std::vector<VulkanAttachmentReference>(0))
			.setDepthStencilAttachment(depthReference)
			.setPreserveAttachments(std::vector<uint32_t>(0));

		// Dependencies
		std::vector<VulkanSubpassDependency> dependencies(0);

		// RenderPass creation
		VulkanRenderPassCreateInfo renderPassInfo;
		renderPassInfo
			.setAttachments(attachments)
			.setSubpasses(subpass)
			.setDependencies(dependencies);

		renderPass = device->createRenderPass(renderPassInfo);
	}

	void Renderer::preparePipelineCache()
	{
		VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo;
		pipelineCache = device->createPipelineCache(pipelineCacheCreateInfo.vkInfo);
	}

	void Renderer::prepareFramebuffers(uint32_t width, uint32_t height)
	{
		std::vector<VkImageView> fbAttachments(2);

		// Depth/Stencil attachment is the same for all frame buffers
		fbAttachments[1] = depthStencil.view;

		VulkanFramebufferCreateInfo frameBufferCreateInfo;
		frameBufferCreateInfo
			.setRenderPass(renderPass.vk)
			.setAttachments(fbAttachments)
			.setWidth(width)
			.setHeight(height)
			.setLayers(1);

		// Create frame buffers for every swap chain image
		frameBuffers.resize(swapchain->imageCount);
		for (uint32_t i = 0; i < frameBuffers.size(); i++)
		{
			fbAttachments[0] = swapchain->buffers[i].view;
			frameBuffers[i] = device->createFrameBuffer(frameBufferCreateInfo.vkInfo);
		}
	}

	void Renderer::submitPrePresentBarrier(VkImage image)
	{
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.pNext = NULL;

		VkResult err = (vkBeginCommandBuffer(prePresentCmdBuffer.vkBuffer, &cmdBufInfo));
		assert(!err);

		VkImageMemoryBarrier prePresentBarrier = {};
		prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		prePresentBarrier.pNext = NULL;
		prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		prePresentBarrier.dstAccessMask = 0;
		prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		prePresentBarrier.image = image;

		vkCmdPipelineBarrier(
			prePresentCmdBuffer.vkBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr, // No memory barriers,
			0, nullptr, // No buffer barriers,
			1, &prePresentBarrier);

		err = (vkEndCommandBuffer(prePresentCmdBuffer.vkBuffer));
		assert(!err);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &prePresentCmdBuffer.vkBuffer;

		err = vkQueueSubmit(*queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!err);
	}
}
