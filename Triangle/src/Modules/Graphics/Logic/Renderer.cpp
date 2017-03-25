#include "stdafx.h"
#include "Modules/Graphics/Logic/Renderer.h"

namespace Graphics
{
	Renderer::Renderer(WindowsContext windows) : context(new VulkanContext("triangle"))
	{
		swapchain = std::make_shared<VulkanSwapChain>(*context);

		queue = std::make_unique<vk::Queue>(context->device.getQueue(context->graphicsQueueIndex, 0));

		// Find supported depth format
		// We prefer 24 bits of depth and 8 bits of stencil, but that may not be supported by all implementations
		std::vector<vk::Format> depthFormats = { vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm };
		for(auto& format : depthFormats)
		{
			if(context->physicalDevice.getFormatProperties(format).optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
			{
				depthFormat = format;
				break;
			}
		}

		swapchain->initSurface(windows.hInstance, windows.window, windows.name);

		vk::CommandPoolCreateInfo cmdPoolInfo = vk::CommandPoolCreateInfo()
			.setQueueFamilyIndex(swapchain->queueNodeIndex)
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		cmdPool = context->device.createCommandPool(cmdPoolInfo);

		vk::CommandBufferBeginInfo cmdBufferBeginInfo = vk::CommandBufferBeginInfo();
		vk::CommandBuffer setupCmdBuffer;
		vk::CommandBufferAllocateInfo info = vk::CommandBufferAllocateInfo()
			.setCommandPool(cmdPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);

		setupCmdBuffer = context->device.allocateCommandBuffers(info).at(0);
		setupCmdBuffer.begin(cmdBufferBeginInfo);

		swapchain->setup(setupCmdBuffer, &windows.width, &windows.height);
		prepareDepthStencil(windows.width, windows.height);
		prepareRenderPass();
		preparePipelineCache();
		prepareFramebuffers(windows.width, windows.height);
		setupCmdBuffer.end();

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&setupCmdBuffer);

		queue->submit(1, &submitInfo, nullptr);
		queue->waitIdle();

		context->device.freeCommandBuffers(cmdPool, 1, &setupCmdBuffer);

		vk::CommandBufferAllocateInfo info2;
		info2.setCommandPool(cmdPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);

		postPresentCmdBuffer = context->device.allocateCommandBuffers(info2).at(0);
		prePresentCmdBuffer = context->device.allocateCommandBuffers(info2).at(0);

		vk::SemaphoreCreateInfo semaphoreCreateInfo;

		// This semaphore ensures that the image is complete
		// before starting to submit again
		presentComplete = context->device.createSemaphore(semaphoreCreateInfo);

		// This semaphore ensures that all commands submitted
		// have been finished before submitting the image to the queue
		renderComplete = context->device.createSemaphore(semaphoreCreateInfo);
	}

	void Renderer::prepareDepthStencil(uint32_t width, uint32_t height)
	{
		vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setFormat(depthFormat)
			.setExtent({ width, height, 1 })
			.setMipLevels(1).setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setTiling(vk::ImageTiling::eOptimal)
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);
		depthStencil.image = context->device.createImage(imageCreateInfo);

		vk::ImageViewCreateInfo imageViewCreateInfo = vk::ImageViewCreateInfo()
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(depthFormat);
		vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);
		imageViewCreateInfo.setSubresourceRange(subresourceRange);

		vk::MemoryRequirements memReqs = context->device.getImageMemoryRequirements(depthStencil.image);

		vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memReqs.size)
			.setMemoryTypeIndex(getMemoryPropertyIndex(vk::MemoryPropertyFlagBits::eDeviceLocal, memReqs));

		depthStencil.mem = context->device.allocateMemory(memoryInfo);

		context->device.bindImageMemory(depthStencil.image, depthStencil.mem, 0);

		imageViewCreateInfo.setImage(depthStencil.image);
		depthStencil.view = context->device.createImageView(imageViewCreateInfo);
	}

	void Renderer::prepareRenderPass()
	{
		// Attachments
		// Color attachment
		std::vector<vk::AttachmentDescription> attachments(2);
		attachments[0]
			.setFormat(colorformat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// Depth attachment
		attachments[1]
			.setFormat(depthFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		// Subpasses
		std::vector<vk::AttachmentReference> colorReference(1);
		// Color reference
		colorReference[0]
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		// Depth reference
		vk::AttachmentReference depthReference;
		depthReference
			.setAttachment(1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		std::vector<vk::SubpassDescription> subpass(1);
		subpass[0]
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(colorReference.data())
			.setPDepthStencilAttachment(&depthReference);
		
		// RenderPass creation
		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo
			.setAttachmentCount(2)
			.setPAttachments(attachments.data())
			.setSubpassCount(1)
			.setPSubpasses(subpass.data());

		renderPass = context->device.createRenderPass(renderPassInfo);
	}

	void Renderer::preparePipelineCache()
	{
		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
		pipelineCache = context->device.createPipelineCache(pipelineCacheCreateInfo);
	}

	void Renderer::prepareFramebuffers(uint32_t width, uint32_t height)
	{
		std::vector<vk::ImageView> fbAttachments(2);

		// Depth/Stencil attachment is the same for all frame buffers
		fbAttachments[1] = depthStencil.view;

		vk::FramebufferCreateInfo frameBufferCreateInfo;
		frameBufferCreateInfo
			.setRenderPass(renderPass)
			.setAttachmentCount(static_cast<uint32_t>(fbAttachments.size()))
			.setPAttachments(fbAttachments.data())
			.setWidth(width)
			.setHeight(height)
			.setLayers(1);

		// Create frame buffers for every swap chain image
		frameBuffers.resize(swapchain->images.size());
		for(uint32_t i = 0; i < frameBuffers.size(); i++)
		{
			fbAttachments[0] = swapchain->buffers[i].view;
			frameBuffers[i] = context->device.createFramebuffer(frameBufferCreateInfo);
		}
	}

	uint32_t Renderer::getMemoryPropertyIndex(vk::MemoryPropertyFlags flag, vk::MemoryRequirements requirements) const
	{
		for(uint32_t i = 0; i < 32; i++)
		{
			if((requirements.memoryTypeBits & 1) == 1)
			{
				if((context->physicalDevice.getMemoryProperties().memoryTypes[i].propertyFlags & flag) == flag)
				{
					return i;
				}
			}
			requirements.memoryTypeBits >>= 1;
		}
		return -1;
	}

	uint32_t Renderer::getCurrentBuffer() const
	{
		return currentBuffer;
	}

	Frame Renderer::getFrame()
	{
		// This needs to be done before the frame is returned since otherwise the wrong buffers (from the previous frame) will be added
		// to the frame. TODO possibly change this so that this call can be done in the submit method. For this, the 'currentBuffer' of this frame must
		// be known before it is retrieved with acquireNextImage.
		swapchain->acquireNextImage(presentComplete, &currentBuffer);
		return Frame{};
	}


	void Renderer::submit(vk::CommandBuffer* buffer) const
	{
		// The submit info structure contains a list of
		// command buffers and semaphores to be submitted to a queue
		// If you want to submit multiple command buffers, pass an array
		vk::SubmitInfo submitInfo;
		vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlagBits::eBottomOfPipe;
		submitInfo
			.setCommandBufferCount(1)
			.setPCommandBuffers(buffer)

			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&presentComplete)
			.setPWaitDstStageMask(&submitPipelineStages)

			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&renderComplete);

		// Submit to the graphics queue
		queue->submit(1, &submitInfo, nullptr);
	}

	void Renderer::prepare()
	{
		submitPostPresentBarrier(swapchain->images[currentBuffer]);
	}

	void Renderer::present()
	{
		submitPrePresentBarrier(swapchain->images[currentBuffer]);

		swapchain->queuePresent(*queue, &currentBuffer, renderComplete);

		queue->waitIdle();
	}

	void Renderer::submitFrame(Frame* frame)
	{
		prepare();
		
		for (auto buffer : frame->buffers) {
			submit(buffer);
		}

		present();
	}

	void Renderer::submitPostPresentBarrier(vk::Image image) const
	{
		vk::CommandBufferBeginInfo cmdBufInfo = vk::CommandBufferBeginInfo();
		postPresentCmdBuffer.begin(&cmdBufInfo);

		vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);

		vk::ImageMemoryBarrier postPresentBarrier = vk::ImageMemoryBarrier()
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eUndefined)
			.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setSubresourceRange(subresourceRange)
			.setImage(image);

		postPresentCmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &postPresentBarrier
		);

		postPresentCmdBuffer.end();

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&postPresentCmdBuffer);
		queue->submit(1, &submitInfo, nullptr);
	}

	void Renderer::submitPrePresentBarrier(vk::Image image) const
	{
		vk::CommandBufferBeginInfo cmdBufInfo = vk::CommandBufferBeginInfo();
		prePresentCmdBuffer.begin(&cmdBufInfo);

		vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);
		vk::ImageMemoryBarrier prePresentBarrier = vk::ImageMemoryBarrier()
			.setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setDstAccessMask(vk::AccessFlagBits::eMemoryRead)
			.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
			.setSubresourceRange(subresourceRange)
			.setImage(image);

		prePresentCmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1,
			&prePresentBarrier
		);

		prePresentCmdBuffer.end();

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&prePresentCmdBuffer);

		queue->submit(1, &submitInfo, nullptr);
	}
}
