#include "GraphicsCore.h"

#include "VulkanWrappers.h"

GraphicsCore::GraphicsCore(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height) : SCREEN_DIMENSIONS(width, height)
{
	instance       = std::make_unique<VulkanInstance>("triangle");
	physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance->getPhysicalDevices()->at(0));

	auto queueProperties = physicalDevice->queueFamilyProperties();
	uint32_t graphicsQueueIndex;
	for (graphicsQueueIndex = 0; graphicsQueueIndex < queueProperties->size(); graphicsQueueIndex++)
	{
		if (queueProperties->at(graphicsQueueIndex).queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;
	}
	assert(graphicsQueueIndex < queueProperties->size());

	device    = std::make_unique<VulkanDevice>(physicalDevice->vkPhysicalDevice, graphicsQueueIndex);
	swapchain = std::make_unique<VulkanSwapChain>(instance->vkInstance, physicalDevice->vkPhysicalDevice, device->vkDevice);
	queue     = std::make_unique<VulkanQueue>(device->queueAt(graphicsQueueIndex));

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

	swapchain->initSurface(hInstance, window, name);

	prepareCommandPool();
	prepareSetupCommandBuffer();

	VulkanCommandBufferBeginInfo cmdBufferBeginInfo;

	setupCmdBuffer.beginCommandBuffer(cmdBufferBeginInfo.vkInfo);
	swapchain->setup(setupCmdBuffer.vkBuffer, &width, &height);
	prepareDepthStencil(width, height);
	prepareRenderPass();
	preparePipelineCache();
	prepareFramebuffers(width, height);
	setupCmdBuffer.end();
	
	VulkanSubmitInfo submitInfo;
	std::vector<VkCommandBuffer> buffers{ setupCmdBuffer.vkBuffer };
	submitInfo.setCommandBuffers(buffers);
	queue->submit(1, submitInfo.vkInfo);
	queue->waitIdle();
	device->freeCommandBuffer(cmdPool, setupCmdBuffer);
	setupCmdBuffer = VK_NULL_HANDLE;

	modules.renderer = std::make_unique<GraphicsRenderingModule>(*device, cmdPool, *swapchain, *queue);
}

void GraphicsCore::render(std::vector<Drawable> d)
{
	modules.renderer->prepare();
	for(auto it = d.begin(); it != d.end(); ++it)
		modules.renderer->submit(*it);
	modules.renderer->present();
}

void GraphicsCore::prepareCommandPool()
{
	VulkanCommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo
		.setQueueFamilyIndex(swapchain->queueNodeIndex)
		.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	cmdPool = device->createCommandPool(cmdPoolInfo.vkInfo);
}

void GraphicsCore::prepareSetupCommandBuffer()
{
	VulkanCommandBufferAllocateInfo info;
	info.setCommandPool(cmdPool)
		.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
		.setCommandBufferCount(1);

	setupCmdBuffer = device->allocateCommandBuffers(info.vkInfo).at(0);
}

void GraphicsCore::prepareDepthStencil(uint32_t width, uint32_t height)
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

void GraphicsCore::prepareRenderPass()
{
	// <Attachments>
	std::vector<VkAttachmentDescription> attachments(2);
	attachments[0] = VulkanAttachmentDescription()
		.setFormat(colorformat)
		.setSamples(VK_SAMPLE_COUNT_1_BIT)
		.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
		.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
		.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
		.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
		.setInitialLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		.setFinalLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		.vkDescription;

	attachments[1] = VulkanAttachmentDescription()
		.setFormat(depthFormat)
		.setSamples(VK_SAMPLE_COUNT_1_BIT)
		.setLoadOp(VK_ATTACHMENT_LOAD_OP_CLEAR)
		.setStoreOp(VK_ATTACHMENT_STORE_OP_STORE)
		.setStencilLoadOp(VK_ATTACHMENT_LOAD_OP_DONT_CARE)
		.setStencilStoreOp(VK_ATTACHMENT_STORE_OP_DONT_CARE)
		.setInitialLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		.setFinalLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		.vkDescription;
	// </Attachments>

	// <Subpasses>
	std::vector<VkAttachmentReference> colorReference(1);
	colorReference[0] = VulkanAttachmentReference()
		.setAttachment(0)
		.setImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		.vkReference;

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
	// </Subpasses>

	// <Dependencies>
	std::vector<VkSubpassDependency> dependencies(0);
	// </Dependencies>

	VulkanRenderPassCreateInfo renderPassInfo;
	renderPassInfo
		.setAttachments(attachments)
		.setSubpasses(subpass)
		.setDependencies(dependencies);

	renderPass = device->createRenderPass(renderPassInfo.vkInfo);
}

void GraphicsCore::preparePipelineCache()
{
	VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo;
	pipelineCache = device->createPipelineCache(pipelineCacheCreateInfo.vkInfo);
}

void GraphicsCore::prepareFramebuffers(uint32_t width, uint32_t height)
{
	std::vector<VkImageView> fbAttachments(2);

	// Depth/Stencil attachment is the same for all frame buffers
	fbAttachments[1] = depthStencil.view;

	VulkanFramebufferCreateInfo frameBufferCreateInfo;
	frameBufferCreateInfo
		.setRenderPass(renderPass)
		.setAttachments(fbAttachments)
		.setWidth(width)
		.setHeight(height)
		.setLayers(1);

	// Create frame buffers for every swap chain image
	frameBuffers.resize(swapchain->imageCount);
	for (uint32_t i = 0; i < frameBuffers.size(); i++)
	{
		fbAttachments[0] = swapchain->buffers[i].view;
		frameBuffers[i]  = device->createFrameBuffer(frameBufferCreateInfo.vkInfo);
	}
}
