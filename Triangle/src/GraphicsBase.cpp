#include "GraphicsBase.h"

#include "VulkanWrappers.h"

GraphicsBase::GraphicsBase(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height) : SCREEN_DIMENSIONS(width, height)
{
	state.swapchain->initSurface(hInstance, window, name);

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
	state.swapchain->setup(setupCmdBuffer.vkBuffer, &width, &height);
	prepareDepthStencil(width, height);
	prepareRenderPass();
	preparePipelineCache();
	prepareFramebuffers(width, height);
	setupCmdBuffer.end();
	
	VulkanSubmitInfo submitInfo;
	std::vector<VkCommandBuffer> buffers{ setupCmdBuffer.vkBuffer };
	submitInfo.setCommandBuffers(buffers);
	state.queue->submit(1, submitInfo.vkInfo);
	state.queue->waitIdle();
	state.device->freeCommandBuffer(state.cmdPool, setupCmdBuffer);

	modules.renderer = std::make_unique<Renderer>(state);
}

void GraphicsBase::render(std::vector<Drawable> d)
{
	modules.renderer->prepare();
	for(auto it = d.begin(); it != d.end(); ++it)
		modules.renderer->submit(*it);
	modules.renderer->present();
}


void GraphicsBase::prepareDepthStencil(uint32_t width, uint32_t height)
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

void GraphicsBase::prepareRenderPass()
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

void GraphicsBase::preparePipelineCache()
{
	VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo;
	state.pipelineCache = state.device->createPipelineCache(pipelineCacheCreateInfo.vkInfo);
}

void GraphicsBase::prepareFramebuffers(uint32_t width, uint32_t height)
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
		state.frameBuffers[i]  = state.device->createFrameBuffer(frameBufferCreateInfo.vkInfo);
	}
}
