#pragma once
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>

#define VERTEX_BUFFER_BIND_ID 0

#include "Shaders.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <math.h>

class VkTriangle
{
public:
	uint32_t width, height;
	std::string name;
	std::unique_ptr<VulkanInstance> instance;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::vector<VkQueueFamilyProperties> queueProperties;
	std::unique_ptr<VulkanDevice> device; // TODO allow more
	std::unique_ptr<VulkanSwapChain> swapchain;

	VkFormat depthFormat;
	std::unique_ptr<VulkanQueue> queue;

	VkCommandPool cmdPool;
	std::vector<VulkanCommandBuffer> drawCmdBuffers;
	// Command buffer used for setup
	VulkanCommandBuffer setupCmdBuffer = VK_NULL_HANDLE;
	// Command buffer for submitting a post present image barrier
	VulkanCommandBuffer postPresentCmdBuffer = VK_NULL_HANDLE;
	// Command buffer for submitting a pre present image barrier
	VulkanCommandBuffer prePresentCmdBuffer = VK_NULL_HANDLE;

	VkMemoryRequirements memReqs;
	VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;
	VkRenderPass renderPass;
	VkPipelineCache pipelineCache;
	std::vector<VkFramebuffer> frameBuffers;
	glm::vec3 rotation = glm::vec3();
	float zoom = -2.5f;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkShaderModule> shaderModules;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };
	struct {
		// Swap chain image presentation
		VkSemaphore presentComplete;
		// Command buffer submission and execution
		VkSemaphore renderComplete;
	} semaphores;

	struct {
		VkBuffer buf;
		VkDeviceMemory mem;
		VkPipelineVertexInputStateCreateInfo vi;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	} vertices;

	struct {
		int count;
		VkBuffer buf;
		VkDeviceMemory mem;
	} indices;

	struct {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo descriptor;
	} uniformDataVS;

	struct {
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	} uboVS;

	struct {
		VkPipeline solid;
	} pipelines;

	struct
	{
		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depthStencil;

	uint32_t currentBuffer = 0;
	
	VkTriangle(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height) : name(name), width(width), height(height)
	{
		initVulkanContext(name);
		swapchain->initSurface(hInstance, window, name);
		createCommandPool();

		// Command buffer begin
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		createSetupCommandBuffer();
		setupCmdBuffer.beginCommandBuffer(cmdBufInfo);
		swapchain->setup(setupCmdBuffer, &width, &height);
		createCommandBuffers();
		initDepthStencil();
		initRenderPass();
		initPipelineCache();
		initFramebuffer();
		flushSetupCommandBuffer();

		createSetupCommandBuffer();
		setupCmdBuffer.beginCommandBuffer(cmdBufInfo);
		initVertices();
		initUniformBuffers();
		initDescriptorSetLayout();
		initPipeline();
		initDescriptorPool();
		updateDescriptorSet();
		initCommandBuffers();
	}

	void render()
	{
		device->waitIdle();

		// Get next image in the swap chain (back/front buffer)
		swapchain->acquireNextImage(semaphores.presentComplete, &currentBuffer);

		// The submit info structure contains a list of
		// command buffers and semaphores to be submitted to a queue
		// If you want to submit multiple command buffers, pass an array
		VkPipelineStageFlags pipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &pipelineStages;
		// The wait semaphore ensures that the image is presented 
		// before we start submitting command buffers agein
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &semaphores.presentComplete;
		// Submit the currently active command buffer
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer].getBuffer();
		// The signal semaphore is used during queue presentation
		// to ensure that the image is not rendered before all
		// commands have been submitted
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &semaphores.renderComplete;

		// Submit to the graphics queue
		queue->submit(1, submitInfo);
		// Present the current buffer to the swap chain
		// This will display the image
		swapchain->queuePresent(*queue, currentBuffer);

		// Add a post present image memory barrier
		// This will transform the frame buffer color attachment back
		// to it's initial layout after it has been presented to the
		// windowing system
		// See buildCommandBuffers for the pre present barrier that 
		// does the opposite transformation 
		VkImageMemoryBarrier postPresentBarrier = {};
		postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		postPresentBarrier.pNext = NULL;
		postPresentBarrier.srcAccessMask = 0;
		postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		postPresentBarrier.image = swapchain->buffers[currentBuffer].image;

		// Use dedicated command buffer from example base class for submitting the post present barrier
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		postPresentCmdBuffer.beginCommandBuffer(cmdBufInfo);

		// Put post present barrier into command buffer
		postPresentCmdBuffer.putPipelineBarrier(postPresentBarrier);
		postPresentCmdBuffer.end();

		// Submit to the queue
		submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &postPresentCmdBuffer.getBuffer();
		queue->submit(1, submitInfo);
		queue->waitIdle();

		device->waitIdle();
	}

private:

	/*
	* Initializes a unique Vulkan Instance for this application.
	* Also initializes physical devices, queue family properties, physical device memory properties, the swapchain, and the queue.
	*/
	void initVulkanContext(std::string name)
	{
		instance = std::make_unique<VulkanInstance>("triangle");
		physicalDevice = std::make_unique<VulkanPhysicalDevice>(instance->getPhysicalDevices().at(0));
		queueProperties = physicalDevice->queueFamilyProperties();

		uint32_t graphicsQueueIndex = getQueueIndexOfType(VK_QUEUE_GRAPHICS_BIT); // TODO factor to seperate class
		device = std::make_unique<VulkanDevice>(physicalDevice->getVkPhysicalDevice(), graphicsQueueIndex);
		swapchain = std::make_unique<VulkanSwapChain>(instance->getVkInstance(), physicalDevice->getVkPhysicalDevice(), device->getVkDevice());
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
	}

	/*
	* Initializes the command pool.
	*/
	void createCommandPool()
	{
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = swapchain->queueNodeIndex;
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cmdPool = device->createCommandPool(cmdPoolInfo);
	}

	/*
	* Initializes the Setup Command Buffer.
	*/
	void createSetupCommandBuffer()
	{
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = cmdPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = 1;
		setupCmdBuffer = device->allocateCommandBuffers(cmdBufAllocateInfo).at(0);
	}

	/*
	* Initializes the Post Present Command Buffer.
	*/
	void createCommandBuffers()
	{
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = cmdPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = swapchain->imageCount;
		drawCmdBuffers = device->allocateCommandBuffers(cmdBufAllocateInfo);

		// TODO one call instead of two needed?
		cmdBufAllocateInfo.commandBufferCount = 1;
		postPresentCmdBuffer = device->allocateCommandBuffers(cmdBufAllocateInfo).at(0);
		prePresentCmdBuffer  = device->allocateCommandBuffers(cmdBufAllocateInfo).at(0);
	}


	/*
	* Initializes the Depth Stencil, the memory barrier, and puts the barrier in the Setup Command Buffer. 
	*/
	void initDepthStencil()
	{
		VkImageCreateInfo image = {};
		image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image.pNext = NULL;
		image.imageType = VK_IMAGE_TYPE_2D;
		image.format = depthFormat;
		image.extent = { width, height, 1 };
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = VK_SAMPLE_COUNT_1_BIT;
		image.tiling = VK_IMAGE_TILING_OPTIMAL;
		image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		image.flags = 0;

		VkMemoryAllocateInfo mem_alloc = {};
		mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext = NULL;
		mem_alloc.allocationSize = 0;
		mem_alloc.memoryTypeIndex = 0;

		VkImageViewCreateInfo depthStencilView = {};
		depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthStencilView.pNext = NULL;
		depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthStencilView.format = depthFormat;
		depthStencilView.flags = 0;
		depthStencilView.subresourceRange = {};
		depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depthStencilView.subresourceRange.baseMipLevel = 0;
		depthStencilView.subresourceRange.levelCount = 1;
		depthStencilView.subresourceRange.baseArrayLayer = 0;
		depthStencilView.subresourceRange.layerCount = 1;

		depthStencil.image = device->createImage(image);
		memReqs = device->getImageMemoryRequirements(depthStencil.image);


		mem_alloc.allocationSize = memReqs.size;

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs.memoryTypeBits & 1) == 1)
			{
				if ((physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
					mem_alloc.memoryTypeIndex = i;
			}
			memReqs.memoryTypeBits >>= 1;
		}

		depthStencil.mem = device->allocateMemory(mem_alloc);

		device->bindImageMemory(depthStencil.image, depthStencil.mem);

		depthStencilView.image = depthStencil.image;
		depthStencil.view = device->createImageView(depthStencilView);
	}


	/*
	* Initializes the Render Pass, including Subpasses.
	*/
	void initRenderPass()
	{
		VkAttachmentDescription attachments[2];
		attachments[0].format = colorformat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		attachments[1].format = depthFormat;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.flags = 0;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = NULL;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorReference;
		subpass.pResolveAttachments = NULL;
		subpass.pDepthStencilAttachment = &depthReference;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = NULL;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.pNext = NULL;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 0;
		renderPassInfo.pDependencies = NULL;

		renderPass = device->createRenderPass(renderPassInfo);
	}


	/*
	* Creates and initializes the Pipeline Cache.
	*/
	void initPipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCache = device->createPipelineCache(pipelineCacheCreateInfo);
	}


	/*
	* Initialize a Frame Buffer for each swapchain image.
	*/
	void initFramebuffer()
	{
		VkImageView fbAttachments[2];

		// Depth/Stencil attachment is the same for all frame buffers
		fbAttachments[1] = depthStencil.view;

		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.pNext = NULL;
		frameBufferCreateInfo.renderPass = renderPass;
		frameBufferCreateInfo.attachmentCount = 2;
		frameBufferCreateInfo.pAttachments = fbAttachments;
		frameBufferCreateInfo.width = width;
		frameBufferCreateInfo.height = height;
		frameBufferCreateInfo.layers = 1;

		// Create frame buffers for every swap chain image
		frameBuffers.resize(swapchain->imageCount);
		for (uint32_t i = 0; i < frameBuffers.size(); i++)
		{
			fbAttachments[0] = swapchain->buffers[i].view;
			frameBuffers[i] = device->createFrameBuffer(frameBufferCreateInfo);
		}
	}

	/*
	* Prepares the semaphores for use.
	*/
	void prepareSemaphores()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = NULL;

		// This semaphore ensures that the image is complete
		// before starting to submit again
		semaphores.presentComplete = device->createSemaphore(semaphoreCreateInfo);

		// This semaphore ensures that all commands submitted
		// have been finished before submitting the image to the queue
		semaphores.renderComplete = device->createSemaphore(semaphoreCreateInfo);
	}

	/*
	* Flushes the Setup Command Buffer.
	*/
	void flushSetupCommandBuffer()
	{
		setupCmdBuffer.end();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &setupCmdBuffer.getBuffer();
		queue->submit(1, submitInfo);

		queue->waitIdle();

		device->freeCommandBuffer(cmdPool, setupCmdBuffer);
		setupCmdBuffer = VK_NULL_HANDLE;
	}

	/*
	* Initializes the vertices and uploads them to the GPU. 
	*/
	void initVertices()
	{
		struct Vertex {
			float pos[3];
			float col[3];
		};

		// Setup vertices
		std::vector<Vertex> vertexBuffer = {
			{ { 1.0f,  1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } }
		};
		int vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);

		// Setup indices
		std::vector<uint32_t> indexBuffer = { 0, 1, 2 };

		int indexBufferSize = indexBuffer.size() * sizeof(uint32_t);

		VkMemoryAllocateInfo memAlloc = {};
		memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAlloc.pNext = NULL;
		memAlloc.allocationSize = 0;
		memAlloc.memoryTypeIndex = 0;
		VkMemoryRequirements memReqs;

		void *data;

		// Generate vertex buffer
		//	Setup
		VkBufferCreateInfo bufInfo = {};
		bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufInfo.pNext = NULL;
		bufInfo.size = vertexBufferSize;
		bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufInfo.flags = 0;
		//	Copy vertex data to VRAM
		memset(&vertices, 0, sizeof(vertices));
		vertices.buf = device->createBuffer(bufInfo);
		memReqs = device->getBufferMemoryRequirements(vertices.buf);
		memAlloc.allocationSize = memReqs.size;

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs.memoryTypeBits & 1) == 1)
			{
				if ((physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				{
					memAlloc.memoryTypeIndex = i;
				}
			}
			memReqs.memoryTypeBits >>= 1;
		}

		memReqs.memoryTypeBits = 1;
		vertices.mem = device->allocateMemory(memAlloc);
		data = device->mapMemory(vertices.mem, memAlloc.allocationSize);
		memcpy(data, vertexBuffer.data(), vertexBufferSize);
		device->unmapMemory(vertices.mem);
		device->bindBufferMemory(vertices.buf, vertices.mem);

		// Generate index buffer
		//	Setup
		VkBufferCreateInfo indexbufferInfo = {};
		indexbufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexbufferInfo.pNext = NULL;
		indexbufferInfo.size = indexBufferSize;
		indexbufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexbufferInfo.flags = 0;
		// Copy index data to VRAM
		memset(&indices, 0, sizeof(indices));
		indices.buf = device->createBuffer(bufInfo);
		memReqs = device->getBufferMemoryRequirements(indices.buf);

		memAlloc.allocationSize = memReqs.size;

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs.memoryTypeBits & 1) == 1)
			{
				if ((physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				{
					memAlloc.memoryTypeIndex = i;
				}
			}
			memReqs.memoryTypeBits >>= 1;
		}

		indices.mem = device->allocateMemory(memAlloc);
		data = device->mapMemory(indices.mem, indexBufferSize);
		memcpy(data, indexBuffer.data(), indexBufferSize);
		device->unmapMemory(indices.mem);
		device->bindBufferMemory(indices.buf, indices.mem);
		indices.count = indexBuffer.size();

		// Binding description
		vertices.bindingDescriptions.resize(1);
		vertices.bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
		vertices.bindingDescriptions[0].stride = sizeof(Vertex);
		vertices.bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		// Attribute descriptions
		// Describes memory layout and shader attribute locations
		vertices.attributeDescriptions.resize(2);
		// Location 0 : Position
		vertices.attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
		vertices.attributeDescriptions[0].location = 0;
		vertices.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertices.attributeDescriptions[0].offset = 0;
		vertices.attributeDescriptions[0].binding = 0;
		// Location 1 : Color
		vertices.attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
		vertices.attributeDescriptions[1].location = 1;
		vertices.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertices.attributeDescriptions[1].offset = sizeof(float) * 3;
		vertices.attributeDescriptions[1].binding = 0;

		// Assign to vertex buffer
		vertices.vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertices.vi.pNext = NULL;
		vertices.vi.vertexBindingDescriptionCount = vertices.bindingDescriptions.size();
		vertices.vi.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
		vertices.vi.vertexAttributeDescriptionCount = vertices.attributeDescriptions.size();
		vertices.vi.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
	}


	/*
	* Initializes the Uniform Buffers of the shader.
	*/
	void initUniformBuffers()
	{
		VkMemoryRequirements memReqs;

		// Vertex shader uniform buffer block
		VkBufferCreateInfo bufferInfo = {};
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = NULL;
		allocInfo.allocationSize = 0;
		allocInfo.memoryTypeIndex = 0;

		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(uboVS);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		// Create a new buffer
		uniformDataVS.buffer = device->createBuffer(bufferInfo);
		// Get memory requirements including size, alignment and memory type 
		memReqs = device->getBufferMemoryRequirements(uniformDataVS.buffer);
		allocInfo.allocationSize = memReqs.size;
		// Gets the appropriate memory type for this type of buffer allocation
		// Only memory types that are visible to the host

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs.memoryTypeBits & 1) == 1)
			{
				if ((physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
					allocInfo.memoryTypeIndex = i;
			}
			memReqs.memoryTypeBits >>= 1;
		}

		// Allocate memory for the uniform buffer
		uniformDataVS.memory = device->allocateMemory(allocInfo);
		// Bind memory to buffer
		device->bindBufferMemory(uniformDataVS.buffer, uniformDataVS.memory);

		// Store information in the uniform's descriptor
		uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
		uniformDataVS.descriptor.offset = 0;
		uniformDataVS.descriptor.range = sizeof(uboVS);

		// Update matrices
		uboVS.projectionMatrix = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 256.0f);
		uboVS.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, zoom));

		uboVS.modelMatrix = glm::mat4();
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Map uniform buffer and update it
		uint8_t *pData;
		VkDeviceSize size2 = sizeof(uboVS);
		pData = (uint8_t*) device->mapMemory(uniformDataVS.memory, size2);
		memcpy(pData, &uboVS, sizeof(uboVS));
		device->unmapMemory(uniformDataVS.memory);
	}


	/*
	* Initializes the Descriptor Set Layout, used to connect shader stages to uniform buffers, image samplers etc. Each shader binding should map to one descriptor set layout.
	*/
	void initDescriptorSetLayout()
	{
		// Binding 0 : Uniform buffer (Vertex shader)
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		layoutBinding.pImmutableSamplers = NULL;

		VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
		descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayout.pNext = NULL;
		descriptorLayout.bindingCount = 1;
		descriptorLayout.pBindings = &layoutBinding;
		
		descriptorSetLayout = device->createDescriptorSetLayout(descriptorLayout);

		// Create the pipeline layout that is used to generate the rendering pipelines that
		// are based on this descriptor set layout
		// In a more complex scenario you would have different pipeline layouts for different
		// descriptor set layouts that could be reused
		VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
		pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pPipelineLayoutCreateInfo.pNext = NULL;
		pPipelineLayoutCreateInfo.setLayoutCount = 1;
		pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

		pipelineLayout = device->createPipelineLayout(pPipelineLayoutCreateInfo);
	}


	/*
	* Initializes and creates the pipeline.
	*/
	void initPipeline()
	{
		// Create our rendering pipeline used in this example
		// Vulkan uses the concept of rendering pipelines to encapsulate
		// fixed states
		// This replaces OpenGL's huge (and cumbersome) state machine
		// A pipeline is then stored and hashed on the GPU making
		// pipeline changes much faster than having to set dozens of 
		// states
		// In a real world application you'd have dozens of pipelines
		// for every shader set used in a scene
		// Note that there are a few states that are not stored with
		// the pipeline. These are called dynamic states and the 
		// pipeline only stores that they are used with this pipeline,
		// but not their states

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};

		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// The layout used for this pipeline
		pipelineCreateInfo.layout = pipelineLayout;
		// Renderpass this pipeline is attached to
		pipelineCreateInfo.renderPass = renderPass;

		// Vertex input state
		// Describes the topoloy used with this pipeline
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		// This pipeline renders vertex data as triangle lists
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		// Solid polygon mode
		rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
		// No culling
		rasterizationState.cullMode = VK_CULL_MODE_NONE;
		rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.depthBiasEnable = VK_FALSE;

		// Color blend state
		// Describes blend modes and color masks
		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		// One blend attachment state
		// Blending is not used in this example
		VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
		blendAttachmentState[0].colorWriteMask = 0xf;
		blendAttachmentState[0].blendEnable = VK_FALSE;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = blendAttachmentState;

		// Viewport state
		VkPipelineViewportStateCreateInfo viewportState = {};

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		// One viewport
		viewportState.viewportCount = 1;
		// One scissor rectangle
		viewportState.scissorCount = 1;

		// Enable dynamic states
		// Describes the dynamic states to be used with this pipeline
		// Dynamic states can be set even after the pipeline has been created
		// So there is no need to create new pipelines just for changing
		// a viewport's dimensions or a scissor box
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		// The dynamic state properties themselves are stored in the command buffer
		std::vector<VkDynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.pDynamicStates = dynamicStateEnables.data();
		dynamicState.dynamicStateCount = dynamicStateEnables.size();

		// Depth and stencil state
		// Describes depth and stenctil test and compare ops
		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		// Basic depth compare setup with depth writes and depth test enabled
		// No stencil used 
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
		depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = depthStencilState.back;

		// Multi sampling state
		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.pSampleMask = NULL;
		// No multi sampling used in this example
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// Load shaders
		VkPipelineShaderStageCreateInfo shaderStages[2] = { {},{} };

		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = loadShader("C:\\Dev\\Vulkan\\data\\shaders\\triangle.vert.spv", *device, VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[0].pName = "main"; // todo : make param
		assert(shaderStages[0].module != NULL);
		shaderModules.push_back(shaderStages[0].module);
		shaderStages[0] = shaderStages[0];

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = loadShader("C:\\Dev\\Vulkan\\data\\shaders\\triangle.frag.spv", *device, VK_SHADER_STAGE_FRAGMENT_BIT);
		shaderStages[1].pName = "main"; // todo : make param
		assert(shaderStages[1].module != NULL);
		shaderModules.push_back(shaderStages[1].module);

		// Assign states
		// Two shader stages
		pipelineCreateInfo.stageCount = 2;
		// Assign pipeline state create information
		pipelineCreateInfo.pVertexInputState = &vertices.vi;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.renderPass = renderPass;
		pipelineCreateInfo.pDynamicState = &dynamicState;

		// Create rendering pipeline
		pipelines.solid = device->createGraphicsPipelines(pipelineCache, pipelineCreateInfo, 1).at(0);
	}

	/*
	* Initializes the Descriptor Pool.
	*/
	void initDescriptorPool()
	{
		// We need to tell the API the number of max. requested descriptors per type
		VkDescriptorPoolSize typeCounts[1];
		// This example only uses one descriptor type (uniform buffer) and only
		// requests one descriptor of this type
		typeCounts[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		typeCounts[0].descriptorCount = 1;
		// For additional types you need to add new entries in the type count list
		// E.g. for two combined image samplers :
		// typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// typeCounts[1].descriptorCount = 2;

		// Create the global descriptor pool
		// All descriptors used in this example are allocated from this pool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.pNext = NULL;
		descriptorPoolInfo.poolSizeCount = 1;
		descriptorPoolInfo.pPoolSizes = typeCounts;
		// Set the max. number of sets that can be requested
		// Requesting descriptors beyond maxSets will result in an error
		descriptorPoolInfo.maxSets = 1;

		descriptorPool = device->createDescriptorPool(descriptorPoolInfo);
	}

	void updateDescriptorSet()
	{
		// Update descriptor sets determining the shader binding points
		// For every binding point used in a shader there needs to be one
		// descriptor set matching that binding point

		VkDescriptorSetAllocateInfo allocInfo2 = {};
		allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo2.descriptorPool = descriptorPool;
		allocInfo2.descriptorSetCount = 1;
		allocInfo2.pSetLayouts = &descriptorSetLayout;

		device->allocateDescriptorSet(allocInfo2, descriptorSet);

		// Binding 0 : Uniform buffer
		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.pBufferInfo = &uniformDataVS.descriptor;
		// Binds this uniform buffer to binding point 0
		writeDescriptorSet.dstBinding = 0;
		device->updateDescriptorSet(writeDescriptorSet);
	}

	void initCommandBuffers()
	{
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.pNext = NULL;

		VkClearValue clearValues[2];
		clearValues[0].color = defaultClearColor;
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
		{
			// Set target frame buffer
			renderPassBeginInfo.framebuffer = frameBuffers.at(i);

			drawCmdBuffers.at(i).beginCommandBuffer(cmdBufInfo);
			drawCmdBuffers.at(i).beginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			// Update dynamic viewport state
			VkViewport viewport = {};
			viewport.height = (float)height;
			viewport.width = (float)width;
			viewport.minDepth = (float) 0.0f;
			viewport.maxDepth = (float) 1.0f;
			drawCmdBuffers.at(i).setViewport(viewport);

			// Update dynamic scissor state
			VkRect2D scissor = {};
			scissor.extent.width = width;
			scissor.extent.height = height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			drawCmdBuffers.at(i).setScissors(scissor);

			// Bind descriptor sets describing shader binding points
			drawCmdBuffers.at(i).bindDescriptorSet(pipelineLayout, descriptorSet);

			// Bind the pipeline with shaders, vertex and index buffers, and draw indexed triangles
			drawCmdBuffers.at(i).bindPipeline(pipelines.solid);
			drawCmdBuffers.at(i).bindVertexBuffers(vertices.buf);
			drawCmdBuffers.at(i).bindIndexBuffers(indices.buf);
			drawCmdBuffers.at(i).drawIndexed(indices.count);

			drawCmdBuffers.at(i).endRenderPass();

			// Add a present memory barrier to the end of the command buffer
			// This will transform the frame buffer color attachment to a
			// new layout for presenting it to the windowing system integration 
			VkImageMemoryBarrier prePresentBarrier = {};
			prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			prePresentBarrier.pNext = NULL;
			prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			prePresentBarrier.dstAccessMask = 0;
			prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			prePresentBarrier.image = swapchain->buffers[i].image;

			drawCmdBuffers.at(i).putPipelineBarrier(prePresentBarrier);
			drawCmdBuffers.at(i).end();
		}
	}

	uint32_t getQueueIndexOfType(VkQueueFlagBits bits)
	{
		uint32_t queueIndex;
		for (queueIndex = 0; queueIndex < queueProperties.size(); queueIndex++)
		{
			if (queueProperties.at(queueIndex).queueFlags & bits)
				break;
		}
		assert(queueIndex < queueProperties.size());
		return queueIndex;
	}

	void submitToQueue(VkQueue& queue, uint32_t count, VkSubmitInfo& info, VkFence& fence)
	{
		VkResult error = vkQueueSubmit(queue, count, &info, fence);
		assert(!error);
	}

	void submitToQueue(VkQueue& queue, uint32_t count, VkSubmitInfo& info)
	{
		VkResult error = vkQueueSubmit(queue, count, &info, VK_NULL_HANDLE);
		assert(!error);
	}
};