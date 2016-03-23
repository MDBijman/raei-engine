#include "VulkanBaseContext.h"

#include "VulkanCommandPoolCreateInfo.h"
#include "VulkanCommandBufferAllocateInfo.h"
#include "VulkanCommandBufferBeginInfo.h"
#include "VulkanImageViewCreateInfo.h"
#include "VulkanImageCreateInfo.h"
#include "VulkanMemoryAllocateInfo.h"
#include "VulkanImageSubresourceRange.h"
#include "VulkanAttachmentDescription.h"
#include "VulkanAttachmentReference.h"
#include "VulkanRenderPassCreateInfo.h"
#include "VulkanSubpassDescription.h"
#include "VulkanPipelineCacheCreateInfo.h"
#include "VulkanFramebufferCreateInfo.h"
#include "VulkanSemaphoreCreateInfo.h"
#include "VulkanSubmitInfo.h"

void VulkanBaseContext::initialize(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height)
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

	device    = std::make_unique<VulkanDevice>(physicalDevice->getVkPhysicalDevice(), graphicsQueueIndex);
	swapchain = std::make_unique<VulkanSwapChain>(instance->vkInstance, physicalDevice->getVkPhysicalDevice(), device->vkDevice);
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
	prepareCommandBuffers();
	prepareDepthStencil(width, height);
	prepareRenderPass();
	preparePipelineCache();
	prepareFramebuffers(width, height);
	prepareSemaphores();
	setupCmdBuffer.end();
	
	VulkanSubmitInfo submitInfo;
	std::vector<VkCommandBuffer> buffers{ setupCmdBuffer.vkBuffer };
	submitInfo.setCommandBuffers(buffers);
	queue->submit(1, submitInfo.vkInfo);
	queue->waitIdle();
	device->freeCommandBuffer(cmdPool, setupCmdBuffer);
	setupCmdBuffer = VK_NULL_HANDLE;
}

void VulkanBaseContext::prepareCommandPool()
{
	VulkanCommandPoolCreateInfo cmdPoolInfo;
	cmdPoolInfo
		.setQueueFamilyIndex(swapchain->queueNodeIndex)
		.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	cmdPool = device->createCommandPool(cmdPoolInfo.vkInfo);
}

void VulkanBaseContext::prepareSetupCommandBuffer()
{
	VulkanCommandBufferAllocateInfo info;
	info.setCommandPool(cmdPool)
		.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
		.setCommandBufferCount(1);

	setupCmdBuffer = device->allocateCommandBuffers(info.vkInfo).at(0);
}

void VulkanBaseContext::prepareCommandBuffers()
{
	VulkanCommandBufferAllocateInfo info;
	info.setCommandPool(cmdPool)
		.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
		.setCommandBufferCount(swapchain->imageCount);

	drawCmdBuffers = device->allocateCommandBuffers(info.vkInfo);

	// TODO one call instead of two needed?
	info.setCommandBufferCount(1);
	postPresentCmdBuffer = device->allocateCommandBuffers(info.vkInfo).at(0);
	prePresentCmdBuffer  = device->allocateCommandBuffers(info.vkInfo).at(0);
}

void VulkanBaseContext::prepareDepthStencil(uint32_t width, uint32_t height)
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

	for (uint32_t i = 0; i < 32; i++)
	{
		if ((memReqs.memoryTypeBits & 1) == 1)
		{
			if ((physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				memoryInfo.setMemoryTypeIndex(i);
		}
		memReqs.memoryTypeBits >>= 1;
	}

	depthStencil.mem = device->allocateMemory(memoryInfo.vkInfo);

	device->bindImageMemory(depthStencil.image, depthStencil.mem);

	imageViewCreateInfo.setImage(depthStencil.image);
	depthStencil.view = device->createImageView(imageViewCreateInfo.vkInfo);
}

void VulkanBaseContext::prepareRenderPass()
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

void VulkanBaseContext::preparePipelineCache()
{
	VulkanPipelineCacheCreateInfo pipelineCacheCreateInfo;
	pipelineCache = device->createPipelineCache(pipelineCacheCreateInfo.vkInfo);
}

void VulkanBaseContext::prepareFramebuffers(uint32_t width, uint32_t height)
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

void VulkanBaseContext::prepareSemaphores()
{
	VulkanSemaphoreCreateInfo semaphoreCreateInfo;

	// This semaphore ensures that the image is complete
	// before starting to submit again
	semaphores.presentComplete = device->createSemaphore(semaphoreCreateInfo.vkInfo);

	// This semaphore ensures that all commands submitted
	// have been finished before submitting the image to the queue
	semaphores.renderComplete = device->createSemaphore(semaphoreCreateInfo.vkInfo);
}