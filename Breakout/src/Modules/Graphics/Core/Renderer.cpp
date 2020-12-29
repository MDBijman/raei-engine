#include "stdafx.h"
#include "Modules/Graphics/Core/Renderer.h"
#include "Modules/Graphics/Core/DefaultRenderpasses.h"

namespace graphics
{
	Renderer::Renderer(WindowsContext windows) : 
		context(new VulkanContext()),
		swapchain(std::make_shared<VulkanSwapChain>(*context)),
		resources(*context, *swapchain, drawPass, frameBuffers),
		factory(resources)
	{
		context->queue = std::make_unique<vk::Queue>(context->device.getQueue(context->graphicsQueueIndex, 0));

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
		context->cmdPool = context->device.createCommandPool(cmdPoolInfo);

		vk::CommandBufferBeginInfo cmdBufferBeginInfo = vk::CommandBufferBeginInfo();
		vk::CommandBuffer setupCmdBuffer;
		vk::CommandBufferAllocateInfo info = vk::CommandBufferAllocateInfo()
			.setCommandPool(context->cmdPool)
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

		context->queue->submit(1, &submitInfo, nullptr);
		context->queue->waitIdle();

		context->device.freeCommandBuffers(context->cmdPool, 1, &setupCmdBuffer);

		vk::CommandBufferAllocateInfo info2;
		info2.setCommandPool(context->cmdPool)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandBufferCount(1);

		postPresentCmdBuffer = context->device.allocateCommandBuffers(info2).at(0);
		prePresentCmdBuffer = context->device.allocateCommandBuffers(info2).at(0);

		vk::SemaphoreCreateInfo semaphoreCreateInfo;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			// This semaphore ensures that the image is complete
			// before starting to submit again
			presentComplete[i] = context->device.createSemaphore(semaphoreCreateInfo);

			// This semaphore ensures that all commands submitted
			// have been finished before submitting the image to the queue
			renderComplete[i] = context->device.createSemaphore(semaphoreCreateInfo);
		}

		vk::FenceCreateInfo fenceCreateInfo = {};
		waitImage = context->device.createFence(fenceCreateInfo);
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
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc
				| vk::ImageUsageFlagBits::eTransferDst);
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
		drawPass = speck::graphics::get_draw_pass(*context, depthFormat, swapchain->colorFormat);
	}

	void Renderer::preparePipelineCache()
	{
		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
		context->pipeline_cache = context->device.createPipelineCache(pipelineCacheCreateInfo);
	}

	void Renderer::prepareFramebuffers(uint32_t width, uint32_t height)
	{
		std::vector<vk::ImageView> fbAttachments(2);

		// Depth/Stencil attachment is the same for all frame buffers
		fbAttachments[1] = depthStencil.view;

		vk::FramebufferCreateInfo frameBufferCreateInfo;
		frameBufferCreateInfo
			.setRenderPass(drawPass)
			.setAttachmentCount(static_cast<uint32_t>(fbAttachments.size()))
			.setPAttachments(fbAttachments.data())
			.setWidth(width)
			.setHeight(height)
			.setLayers(1);

		// Create frame buffers for every swap chain image
		frameBuffers.resize(swapchain->images.size());
		for (uint32_t i = 0; i < frameBuffers.size(); i++)
		{
			fbAttachments[0] = swapchain->buffers[i].view;
			frameBuffers[i] = context->device.createFramebuffer(frameBufferCreateInfo);
		}
	}

	uint32_t Renderer::getMemoryPropertyIndex(vk::MemoryPropertyFlags flag, vk::MemoryRequirements requirements) const
	{
		for (uint32_t i = 0; i < 32; i++)
		{
			if ((requirements.memoryTypeBits & 1) == 1)
			{
				if ((context->physicalDevice.getMemoryProperties().memoryTypes[i].propertyFlags & flag) == flag)
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
		swapchain->acquireNextImage(presentComplete[current_frame], waitImage, &currentBuffer);
		context->device.waitForFences(waitImage, VK_TRUE, 1000000000000);
		context->device.resetFences(1, &waitImage);
		return Frame(getCurrentBuffer());
	}

	void Renderer::submit(vk::CommandBuffer* buffer) const
	{
		vk::SubmitInfo submitInfo;
		submitInfo
			.setCommandBufferCount(1)
			.setPCommandBuffers(buffer);

		context->queue->submit(1, &submitInfo, nullptr);
	}

	void Renderer::submit(vk::CommandBuffer* buffer, vk::Semaphore waitFor, vk::Semaphore signal) const
	{
		vk::PipelineStageFlags waitBits = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo;
		submitInfo
			.setCommandBufferCount(1)
			.setPCommandBuffers(buffer)
			.setWaitSemaphoreCount(1)
			.setPWaitSemaphores(&waitFor)
			.setSignalSemaphoreCount(1)
			.setPSignalSemaphores(&signal)
			.setPWaitDstStageMask(&waitBits);

		context->queue->submit(1, &submitInfo, nullptr);
	}


	void Renderer::prepare()
	{
		submitPostPresentBarrier(swapchain->images[currentBuffer]);
	}

	void Renderer::submitFrame(Frame* frame)
	{
		prepare();

		for (int i = 0; i < frame->buffers.size() - 1; i++)
		{
			submit(frame->buffers[i]);
		}

		submit(frame->buffers.back(), presentComplete[current_frame], renderComplete[current_frame]);

		context->queue->waitIdle();

		present();

		current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::present()
	{
		submitPrePresentBarrier(swapchain->images[currentBuffer]);

		swapchain->queuePresent(*context->queue, &currentBuffer, renderComplete[current_frame]);

		context->queue->waitIdle();
	}

	void Renderer::submitPostPresentBarrier(vk::Image image) const
	{
		vk::CommandBufferBeginInfo cmdBufInfo = vk::CommandBufferBeginInfo();
		postPresentCmdBuffer.begin(&cmdBufInfo);
		
		// Image buffer

		vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);

		// Barrier for clearing color buffer
		vk::ImageMemoryBarrier prepareClearBarrier = vk::ImageMemoryBarrier()
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eUndefined)
			.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
			.setSubresourceRange(subresourceRange)
			.setImage(image);
		postPresentCmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &prepareClearBarrier
		);

		// Clear color buffer
		auto clearColorValues = std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f};
		postPresentCmdBuffer.clearColorImage(image, vk::ImageLayout::eTransferDstOptimal,
			&vk::ClearColorValue(clearColorValues), 1, &subresourceRange);

		// Barrier for changing color buffer layout back
		vk::ImageMemoryBarrier postPresentBarrier = vk::ImageMemoryBarrier()
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
			.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setSubresourceRange(subresourceRange)
			.setImage(image);
		postPresentCmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &postPresentBarrier
		);

		// Depth buffer

		vk::ImageSubresourceRange depthSubresourceRange = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1);

		// Barrier for clearing depth buffer
		vk::ImageMemoryBarrier depthStencilClearBarrier = vk::ImageMemoryBarrier()
			.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eUndefined)
			.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
			.setSubresourceRange(depthSubresourceRange)
			.setImage(depthStencil.image);
		postPresentCmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eAllCommands, // TODO try back to eEarlyFragmentTest
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &depthStencilClearBarrier
		);

		// Clear depth buffer
		auto clearDepthValues = vk::ClearDepthStencilValue(0.0f, 0);
		postPresentCmdBuffer.clearDepthStencilImage(depthStencil.image, vk::ImageLayout::eTransferDstOptimal,
			clearDepthValues, depthSubresourceRange);

		// Barrier for changing depth buffer layout back
		vk::ImageMemoryBarrier depthLayoutBarrier = vk::ImageMemoryBarrier()
			.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite)
			.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
			.setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setSubresourceRange(depthSubresourceRange)
			.setImage(depthStencil.image);
		postPresentCmdBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eAllCommands, // TODO try back to eEarlyFragmentTest
			vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &depthLayoutBarrier
		);

		postPresentCmdBuffer.end();

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&postPresentCmdBuffer);
		context->queue->submit(1, &submitInfo, nullptr);
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

		context->queue->submit(1, &submitInfo, nullptr);
	}
}
