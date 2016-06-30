#include "Renderer.h"

#include "VulkanWrappers.h"
#include "Vertex.h"
#include "StateBucket.h"

namespace Graphics
{
	Renderer::Renderer(WindowsContext context) : state()
	{
		state.swapchain->initSurface(context.hInstance, context.window, context.name);

		VulkanCommandPoolCreateInfo cmdPoolInfo;
		cmdPoolInfo
			.setQueueFamilyIndex(state.swapchain->queueNodeIndex)
			.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		state.cmdPool = state.device->createCommandPool(cmdPoolInfo.vkInfo);

		VulkanCommandBufferBeginInfo cmdBufferBeginInfo;
		VulkanCommandBuffer setupCmdBuffer;
		VulkanCommandBufferAllocateInfo info;
		info.setCommandPool(state.cmdPool)
			.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
			.setCommandBufferCount(1);

		setupCmdBuffer = state.device->allocateCommandBuffers(info.vkInfo).at(0);
		setupCmdBuffer.beginCommandBuffer(cmdBufferBeginInfo.vkInfo);
		state.swapchain->setup(setupCmdBuffer.vkBuffer, &context.width, &context.height);
		prepareDepthStencil(context.width, context.height);
		prepareRenderPass();
		preparePipelineCache();
		prepareFramebuffers(context.width, context.height);
		setupCmdBuffer.end();

		VulkanSubmitInfo submitInfo;
		std::vector<VkCommandBuffer> buffers{ setupCmdBuffer.vkBuffer };
		submitInfo.setCommandBuffers(buffers);
		state.queue->submit(1, submitInfo.vkInfo);
		state.queue->waitIdle();
		state.device->freeCommandBuffer(state.cmdPool, setupCmdBuffer);

		VulkanCommandBufferAllocateInfo info2;
		info2.setCommandPool(state.cmdPool)
			.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
			.setCommandBufferCount(1);

		postPresentCmdBuffer = state.device->allocateCommandBuffers(info2.vkInfo).at(0);
		prePresentCmdBuffer = state.device->allocateCommandBuffers(info2.vkInfo).at(0);

		VulkanSemaphoreCreateInfo semaphoreCreateInfo;

		// This semaphore ensures that the image is complete
		// before starting to submit again
		presentComplete = state.device->createSemaphore(semaphoreCreateInfo.vkInfo);

		// This semaphore ensures that all commands submitted
		// have been finished before submitting the image to the queue
		renderComplete = state.device->createSemaphore(semaphoreCreateInfo.vkInfo);
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
		state.swapchain->acquireNextImage(presentComplete, &currentBuffer);
		submitPostPresentBarrier(state.swapchain->buffers[currentBuffer].image);
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
		state.queue->submit(1, submitInfo.vkInfo);
	}

	void Renderer::present()
	{
		submitPrePresentBarrier(state.swapchain->buffers[currentBuffer].image);

		state.swapchain->queuePresent(*state.queue, currentBuffer, renderComplete);

		VkResult err = vkQueueWaitIdle(*state.queue);
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

		err = vkQueueSubmit(*state.queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!err);
	}

	void Renderer::prepareDepthStencil(uint32_t width, uint32_t height)
	{
		VulkanImageCreateInfo imageCreateInfo;
		imageCreateInfo
			.setImageType(VK_IMAGE_TYPE_2D)
			.setFormat(state.depthFormat)
			.setExtent({ width, height, 1 })
			.setMipLevels(1).setArrayLayers(1)
			.setSamples(VK_SAMPLE_COUNT_1_BIT)
			.setTiling(VK_IMAGE_TILING_OPTIMAL)
			.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			.setFlags(0);
		state.depthStencil.image = state.device->createImage(imageCreateInfo.vkInfo);

		VulkanImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo
			.setViewType(VK_IMAGE_VIEW_TYPE_2D)
			.setFormat(state.depthFormat)
			.setFlags(0);
		VulkanImageSubresourceRange subresourceRange;
		subresourceRange
			.setAspectMask(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);
		imageViewCreateInfo.setSubresourceRange(subresourceRange.vkRange);

		VkMemoryRequirements memReqs = state.device->getImageMemoryRequirements(state.depthStencil.image);

		VulkanMemoryAllocateInfo memoryInfo;
		memoryInfo.setAllocationSize(memReqs.size);
		memoryInfo.setMemoryTypeIndex(state.physicalDevice->getMemoryPropertyIndex(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memReqs));

		state.depthStencil.mem = state.device->allocateMemory(memoryInfo.vkInfo);

		state.device->bindImageMemory(state.depthStencil.image, state.depthStencil.mem);

		imageViewCreateInfo.setImage(state.depthStencil.image);
		state.depthStencil.view = state.device->createImageView(imageViewCreateInfo.vkInfo);
	}

	void Renderer::prepareRenderPass()
	{
		// Attachments
		// Color attachment
		std::vector<VkAttachmentDescription> attachments(2);
		attachments[0] = VulkanAttachmentDescription()
			.setFormat(state.colorformat)
			.setSamples(VK_SAMPLE_COUNT_1_BIT)
			.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
			.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
			.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
			.setInitialLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.setFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.vkDescription;

		// Depth attachment
		attachments[1] = VulkanAttachmentDescription()
			.setFormat(state.depthFormat)
			.setSamples(VK_SAMPLE_COUNT_1_BIT)
			.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
			.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
			.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
			.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
			.setInitialLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.setFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			.vkDescription;

		// Subpasses
		std::vector<VkAttachmentReference> colorReference(1);
		// Color reference
		colorReference[0] = VulkanAttachmentReference()
			.setAttachment(0)
			.setImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.vkReference;

		// Depth reference
		VulkanAttachmentReference depthReference;
		depthReference
			.setAttachment(1)
			.setImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		std::vector<VkSubpassDescription> subpass(1);
		subpass[0] = VulkanSubpassDescription()
			.setPipelineBindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS)
			.setFlags(0)
			.setInputAttachments(std::vector<VkAttachmentReference>(0))
			.setColorAttachments(colorReference)
			.setResolveAttachments(std::vector<VkAttachmentReference>(0))
			.setDepthStencilAttachment(depthReference.vkReference)
			.setPreserveAttachments(std::vector<uint32_t>(0))
			.vkDescription;

		// Dependencies
		std::vector<VkSubpassDependency> dependencies(0);

		// RenderPass creation
		VulkanRenderPassCreateInfo renderPassInfo;
		renderPassInfo
			.setAttachments(attachments)
			.setSubpasses(subpass)
			.setDependencies(dependencies);

		state.renderPass = state.device->createRenderPass(renderPassInfo.vkInfo);
	}

	void Renderer::preparePipelineCache()
	{
		VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo;
		state.pipelineCache = state.device->createPipelineCache(pipelineCacheCreateInfo.vkInfo);
	}

	void Renderer::prepareFramebuffers(uint32_t width, uint32_t height)
	{
		std::vector<VkImageView> fbAttachments(2);

		// Depth/Stencil attachment is the same for all frame buffers
		fbAttachments[1] = state.depthStencil.view;

		VulkanFramebufferCreateInfo frameBufferCreateInfo;
		frameBufferCreateInfo
			.setRenderPass(state.renderPass)
			.setAttachments(fbAttachments)
			.setWidth(width)
			.setHeight(height)
			.setLayers(1);

		// Create frame buffers for every swap chain image
		state.frameBuffers.resize(state.swapchain->imageCount);
		for (uint32_t i = 0; i < state.frameBuffers.size(); i++)
		{
			fbAttachments[0] = state.swapchain->buffers[i].view;
			state.frameBuffers[i] = state.device->createFrameBuffer(frameBufferCreateInfo.vkInfo);
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

		err = vkQueueSubmit(*state.queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!err);
	}
}
