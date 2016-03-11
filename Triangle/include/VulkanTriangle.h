#pragma once
#include <vector>
#include <iostream>
#include <chrono>

#define VERTEX_BUFFER_BIND_ID 0

#include "BaseApplication.h"
#include "Shaders.h"

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <math.h>
#define deg_to_rad(deg) deg * float(M_PI / 180)

class VkTriangle : public VA::BaseApplication
{
public:
	uint32_t width, height;
	std::string name;
	VulkanInstance							instance;
	VulkanPhysicalDevice					physicalDevice;
	std::vector<VkQueueFamilyProperties>	queueProperties;
	VulkanDevice							device; // TODO allow more
	std::unique_ptr<VulkanSwapChain>		swapchain;

	VkFormat depthFormat;
	VkQueue queue;
	VkCommandPool cmdPool;
	std::vector<VkCommandBuffer> setupCmdBuffer;
	std::vector<VkCommandBuffer> drawCmdBuffers;
	std::vector<VkCommandBuffer> postPresentCmdBuffer;
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

	HWND window;
	HINSTANCE windowInstance;
	float frameTimer = 1.0f;
	float timerSpeed = 0.25f;
	float timer = 0.0f;
	bool paused = false;

	uint32_t currentBuffer = 0;
	
	VkTriangle(HINSTANCE hInstance, WNDPROC WndProc, std::string name, uint32_t width, uint32_t height) : name(name), width(width), height(height)
	{
		initVulkanContext(name);
		initWin32Context(hInstance, WndProc, name, width, height);
		swapchain->initSwapChain(windowInstance, window, name);

		initCommandPool();
		initSetupCmdBuffer();

		// Command buffer begin
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginCommandBuffer(setupCmdBuffer.at(0), cmdBufInfo);

		swapchain->setup(setupCmdBuffer.at(0), &width, &height);

		initPostPresentCmdBuffer();
		initDepthStencil();
		initRenderPass();
		initPipelineCache();
		initFramebuffer();
		flushSetupCmdBuffer();
		beginSetupCmdBuffer();
		initVertices();
		initUniformBuffers();
		initDescriptorSetLayout();
		initPipeline();
		initDescriptorPool();
		updateDescriptorSet();
		initCommandBuffers();
	}

	void loop()
	{
		MSG msg;
		while (TRUE)
		{
			auto tStart = std::chrono::high_resolution_clock::now();
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			//////
			vkDeviceWaitIdle(device);
			VkResult err;
			VkSemaphore presentCompleteSemaphore;
			VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo = {
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, // sType
				NULL, // pNext&postPresentBarrier
				0 // flags
			};

			err = vkCreateSemaphore(device, &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);
			assert(!err);

			// Get next image in the swap chain (back/front buffer)
			swapchain->acquireNextImage(presentCompleteSemaphore, &currentBuffer);

			// The submit info structure contains a list of
			// command buffers and semaphores to be submitted to a queue
			// If you want to submit multiple command buffers, pass an array
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];

			// Submit to the graphics queue
			submitToQueue(queue, 1, submitInfo);

			// Present the current buffer to the swap chain
			// This will display the image
			swapchain->queuePresent(queue, currentBuffer);

			vkDestroySemaphore(device, presentCompleteSemaphore, nullptr);

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

			err = vkBeginCommandBuffer(postPresentCmdBuffer.at(0), &cmdBufInfo);
			assert(!err);

			// Put post present barrier into command buffer
			vkCmdPipelineBarrier(
				postPresentCmdBuffer.at(0),
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &postPresentBarrier);

			err = vkEndCommandBuffer(postPresentCmdBuffer.at(0));
			assert(!err);

			// Submit to the queue
			submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &postPresentCmdBuffer.at(0);

			err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
			assert(!err);

			err = vkQueueWaitIdle(queue);
			assert(!err);
			vkDeviceWaitIdle(device);
			////////
			auto tEnd = std::chrono::high_resolution_clock::now();
			auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
			frameTimer = (float)tDiff / 1000.0f;
			// Convert to clamped timer value
			if (!paused)
			{
				timer += timerSpeed * frameTimer;
				if (timer > 1.0)
				{
					timer -= 1.0f;
				}
			}
		}
	}

private:

	/*
	* Initializes a unique Vulkan Instance for this application.
	* Also initializes physical devices, queue family properties, physical device memory properties, the swapchain, and the queue.
	*/
	void initVulkanContext(std::string name)
		initSwapchain();
	{
		instance = VulkanInstance();
		physicalDevice = instance.physicalDevices().at(0);
		queueProperties = physicalDevice.queueFamilyProperties();
		memoryProperties = physicalDevice.memoryProperties();

		uint32_t graphicsQueueIndex = getQueueIndexOfType(VK_QUEUE_GRAPHICS_BIT);
		device = VulkanDevice(physicalDevice, graphicsQueueIndex);

		swapchain = std::make_unique<VulkanSwapChain>(instance, physicalDevice, device);

		queue = device.queueAt(graphicsQueueIndex);
		// Find supported depth format
		// We prefer 24 bits of depth and 8 bits of stencil, but that may not be supported by all implementations
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
		for (auto& format : depthFormats)
		{
			if (physicalDevice.formatProperties(format).optimalTilingFeatures && VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				depthFormat = format;
				break;
			}
		}
	}

	/*
	* Creates and initializes a Win32 Context.
	*/
	void initWin32Context(HINSTANCE hInstance, WNDPROC WndProc, std::string name, int width, int height)
	{
		bool fullscreen = false;

		// Check command line arguments
		for (int32_t i = 0; i < __argc; i++)
		{
			if (__argv[i] == std::string("-fullscreen"))
			{
				fullscreen = true;
			}
		}

		WNDCLASSEX wndClass;

		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = name.c_str();
		wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&wndClass))
		{
			std::cout << "Could not register window class!\n";
			fflush(stdout);
			exit(1);
		}

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		if (fullscreen)
		{
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = screenWidth;
			dmScreenSettings.dmPelsHeight = screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if ((width != screenWidth) && (height != screenHeight))
			{
				if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					if (MessageBox(NULL, "Fullscreen Mode not supported!\n Switch to window mode?", "Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
						fullscreen = FALSE;
					else
						exit(1);
				}
			}

		}

		DWORD dwExStyle;
		DWORD dwStyle;

		if (fullscreen)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		}

		RECT windowRect;
		if (fullscreen)
		{
			windowRect.left = (long)0;
			windowRect.right = (long)screenWidth;
			windowRect.top = (long)0;
			windowRect.bottom = (long)screenHeight;
		}
		else
		{
			windowRect.left = (long)screenWidth / 2 - width / 2;
			windowRect.right = (long)width;
			windowRect.top = (long)screenHeight / 2 - height / 2;
			windowRect.bottom = (long)height;
		}

		AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

		window = CreateWindowEx(0,
			name.c_str(),
			name.c_str(),
			//		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
			dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			windowRect.left,
			windowRect.top,
			windowRect.right,
			windowRect.bottom,
			NULL,
			NULL,
			hInstance,
			NULL);

		if (!window)
		{
			printf("Could not create window!\n");
			fflush(stdout);
			exit(1);
		}

		ShowWindow(window, SW_SHOW);
		SetForegroundWindow(window);
		SetFocus(window);
	}

	/*
	* Initializes the command pool.
	*/
	void initCommandPool()
	{
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = swapchain->queueNodeIndex;
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cmdPool = createCommandPool(cmdPoolInfo);
	}

	/*
	* Initializes the Setup Command Buffer.
	*/
	void initSetupCmdBuffer()
	{
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = cmdPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = 1;
		setupCmdBuffer = allocateCommandBuffers(cmdBufAllocateInfo);
	}

	/*
	* Initializes the Post Present Command Buffer.
	*/
	void initPostPresentCmdBuffer()
	{
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = cmdPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = (uint32_t)swapchain->imageCount;
		drawCmdBuffers = allocateCommandBuffers(cmdBufAllocateInfo);

		cmdBufAllocateInfo.commandBufferCount = 1;
		postPresentCmdBuffer = allocateCommandBuffers(cmdBufAllocateInfo);
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

		depthStencil.image = createImage(image);
		memReqs = getImageMemoryRequirements(depthStencil.image);


		mem_alloc.allocationSize = memReqs.size;

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs.memoryTypeBits & 1) == 1)

			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				{
					mem_alloc.memoryTypeIndex = i;
				}
			}
			memReqs.memoryTypeBits >>= 1;
		}


		depthStencil.mem = allocateMemory(mem_alloc);
		bindImageMemory(depthStencil.image, depthStencil.mem);

		// Create an image barrier object
		VkImageMemoryBarrier imageMemoryBarrier = {};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = NULL;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;	
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.image = depthStencil.image;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.layerCount = 1;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		// Put barrier on top
		VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		// Put barrier inside setup command buffer
		vkCmdPipelineBarrier(setupCmdBuffer.at(0), srcStageFlags, destStageFlags,	0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		depthStencilView.image = depthStencil.image;
		depthStencil.view = createImageView(depthStencilView);
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

		renderPass = createRenderPass(renderPassInfo);
	}


	/*
	* Creates and initializes the Pipeline Cache.
	*/
	void initPipelineCache()
	{
		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		pipelineCache = createPipelineCache(pipelineCacheCreateInfo);
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
			frameBuffers[i] = createFrameBuffer(frameBufferCreateInfo);
		}
	}


	/*
	* Flushes the Setup Command Buffer.
	*/
	void flushSetupCmdBuffer()
	{
		VkResult errorStatus = vkEndCommandBuffer(setupCmdBuffer.at(0));
		assert(!errorStatus);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &setupCmdBuffer.at(0);

		errorStatus = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!errorStatus);

		errorStatus = vkQueueWaitIdle(queue);
		assert(!errorStatus);

		freeCommandBuffer(cmdPool, setupCmdBuffer);
		setupCmdBuffer.at(0) = VK_NULL_HANDLE;
	}


	/*
	* Begins the Setup Command Buffer.
	*/
	void beginSetupCmdBuffer()
	{
		VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
		cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocateInfo.commandPool = cmdPool;
		cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocateInfo.commandBufferCount = 1;
		setupCmdBuffer = allocateCommandBuffers(cmdBufAllocateInfo);

		// todo : Command buffer is also started here, better put somewhere else
		// todo : Check if necessaray at all...
		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// todo : check null handles, flags?

		beginCommandBuffer(setupCmdBuffer.at(0), cmdBufInfo);
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
		VkMemoryRequirements memReqs2;

		VkResult err;
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
		vertices.buf = createBuffer(bufInfo);
		memReqs2 = getBufferMemoryRequirements(vertices.buf);
		memAlloc.allocationSize = memReqs2.size;

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs2.memoryTypeBits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				{
					memAlloc.memoryTypeIndex = i;
				}
			}
			memReqs2.memoryTypeBits >>= 1;
		}

		memReqs2.memoryTypeBits = 1;
		vertices.mem = allocateMemory(memAlloc);
		data = mapMemory(vertices.mem, memAlloc.allocationSize);
		memcpy(data, vertexBuffer.data(), vertexBufferSize);
		unmapMemory(vertices.mem);
		bindBufferMemory(vertices.buf, vertices.mem);

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
		indices.buf = createBuffer(bufInfo);
		memReqs2 = getBufferMemoryRequirements(indices.buf);

		memAlloc.allocationSize = memReqs2.size;

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs2.memoryTypeBits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				{
					memAlloc.memoryTypeIndex = i;
				}
			}
			memReqs2.memoryTypeBits >>= 1;
		}

		indices.mem = allocateMemory(memAlloc);
		VkDeviceSize size = indexBufferSize;
		data = mapMemory(indices.mem, size);
		memcpy(data, indexBuffer.data(), indexBufferSize);
		unmapMemory(indices.mem);
		bindBufferMemory(indices.buf, indices.mem);
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
		VkMemoryRequirements memReqs3;

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
		uniformDataVS.buffer = createBuffer(bufferInfo);
		// Get memory requirements including size, alignment and memory type 
		memReqs3 = getBufferMemoryRequirements(uniformDataVS.buffer);
		allocInfo.allocationSize = memReqs3.size;
		// Gets the appropriate memory type for this type of buffer allocation
		// Only memory types that are visible to the host

		for (uint32_t i = 0; i < 32; i++)
		{
			if ((memReqs3.memoryTypeBits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				{
					allocInfo.memoryTypeIndex = i;
				}
			}
			memReqs3.memoryTypeBits >>= 1;
		}

		// Allocate memory for the uniform buffer
		uniformDataVS.memory = allocateMemory(allocInfo);
		// Bind memory to buffer
		bindBufferMemory(uniformDataVS.buffer, uniformDataVS.memory);

		// Store information in the uniform's descriptor
		uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
		uniformDataVS.descriptor.offset = 0;
		uniformDataVS.descriptor.range = sizeof(uboVS);

		// Update matrices
		uboVS.projectionMatrix = glm::perspective(deg_to_rad(60.0f), (float)width / (float)height, 0.1f, 256.0f);

		uboVS.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, zoom));

		uboVS.modelMatrix = glm::mat4();
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, deg_to_rad(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, deg_to_rad(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, deg_to_rad(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Map uniform buffer and update it
		uint8_t *pData;
		VkDeviceSize size2 = sizeof(uboVS);
		pData = (uint8_t*) mapMemory(uniformDataVS.memory, size2);
		memcpy(pData, &uboVS, sizeof(uboVS));
		unmapMemory(uniformDataVS.memory);
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
		
		descriptorSetLayout = createDescriptorSetLayout(descriptorLayout);

		// Create the pipeline layout that is used to generate the rendering pipelines that
		// are based on this descriptor set layout
		// In a more complex scenario you would have different pipeline layouts for different
		// descriptor set layouts that could be reused
		VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
		pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pPipelineLayoutCreateInfo.pNext = NULL;
		pPipelineLayoutCreateInfo.setLayoutCount = 1;
		pPipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

		pipelineLayout = createPipelineLayout(pPipelineLayoutCreateInfo);
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
		shaderStages[0].module = loadShader("C:\\Dev\\Vulkan\\data\\shaders\\triangle.vert.spv", device, VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[0].pName = "main"; // todo : make param
		assert(shaderStages[0].module != NULL);
		shaderModules.push_back(shaderStages[0].module);
		shaderStages[0] = shaderStages[0];

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = loadShader("C:\\Dev\\Vulkan\\data\\shaders\\triangle.frag.spv", device, VK_SHADER_STAGE_FRAGMENT_BIT);
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
		pipelines.solid = createGraphicsPipelines(pipelineCache, pipelineCreateInfo, 1).at(0);
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

		descriptorPool = createDescriptorPool(descriptorPoolInfo);
	}

	void updateDescriptorSet()
	{
		// Update descriptor sets determining the shader binding points
		// For every binding point used in a shader there needs to be one
		// descriptor set matching that binding point
		VkWriteDescriptorSet writeDescriptorSet = {};

		VkDescriptorSetAllocateInfo allocInfo2 = {};
		allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo2.descriptorPool = descriptorPool;
		allocInfo2.descriptorSetCount = 1;
		allocInfo2.pSetLayouts = &descriptorSetLayout;

		allocateDescriptorSet(allocInfo2, descriptorSet);

		// Binding 0 : Uniform buffer
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = descriptorSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSet.pBufferInfo = &uniformDataVS.descriptor;
		// Binds this uniform buffer to binding point 0
		writeDescriptorSet.dstBinding = 0;

		BaseApplication::updateDescriptorSet(writeDescriptorSet);
	}
	void initCommandBuffers()
	{
		VkCommandBufferBeginInfo cmdBufInfo3 = {};
		cmdBufInfo3.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo3.pNext = NULL;

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
			renderPassBeginInfo.framebuffer = frameBuffers[i];

			VkResult err = vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo3);
			assert(!err);

			vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			// Update dynamic viewport state
			VkViewport viewport = {};
			viewport.height = (float)height;
			viewport.width = (float)width;
			viewport.minDepth = (float) 0.0f;
			viewport.maxDepth = (float) 1.0f;
			vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

			// Update dynamic scissor state
			VkRect2D scissor = {};
			scissor.extent.width = width;
			scissor.extent.height = height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

			// Bind descriptor sets describing shader binding points
			vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

			// Bind the rendering pipeline (including the shaders)
			vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.solid);

			// Bind triangle vertices
			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, &vertices.buf, offsets);

			// Bind triangle indices
			vkCmdBindIndexBuffer(drawCmdBuffers[i], indices.buf, 0, VK_INDEX_TYPE_UINT32);

			// Draw indexed triangle
			vkCmdDrawIndexed(drawCmdBuffers[i], indices.count, 1, 0, 0, 1);

			vkCmdEndRenderPass(drawCmdBuffers[i]);

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

			VkImageMemoryBarrier *pMemoryBarrier = &prePresentBarrier;
			vkCmdPipelineBarrier(
				drawCmdBuffers[i],
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &prePresentBarrier);

			err = vkEndCommandBuffer(drawCmdBuffers[i]);
			assert(!err);
		}

	}
};