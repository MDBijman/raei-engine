#include <VulkanTriangle.h>

VulkanTriangle::VulkanTriangle(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height) : name(name), width(width), height(height)
{
	context.initialize(hInstance, window, name, width, height);

	VulkanCommandBufferAllocateInfo info;
	info.setCommandPool(context.cmdPool)
		.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
		.setCommandBufferCount(1);

	VulkanCommandBuffer setupCmdBuffer = context.device->allocateCommandBuffers(info.vkInfo).at(0);

	prepareVertices();
	prepareUniformBuffers();
	prepareDescriptorSetLayout();
	preparePipeline();
	prepareDescriptorPool();
	updateDescriptorSet();
	prepareCommandBuffers();
}

void VulkanTriangle::render()
{
	context.device->waitIdle();

	// Get next image in the swap chain (back/front buffer)
	context.swapchain->acquireNextImage(context.semaphores.presentComplete, &currentBuffer);

	// The submit info structure contains a list of
	// command buffers and semaphores to be submitted to a queue
	// If you want to submit multiple command buffers, pass an array
	VkPipelineStageFlags pipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VulkanSubmitInfo submitInfo;
	std::vector<VkSemaphore> waitSemaphores{ context.semaphores.presentComplete };
	std::vector<VkSemaphore> signalSemaphores{ context.semaphores.renderComplete };
	std::vector<VkCommandBuffer> commandBuffers{ context.drawCmdBuffers[currentBuffer].vkBuffer };
	submitInfo
		.setDstStageMask(&pipelineStages)
		.setWaitSemaphores(waitSemaphores)
		.setCommandBuffers(commandBuffers)
		.setSignalSemaphores(signalSemaphores);

	// Submit to the graphics queue
	context.queue->submit(1, submitInfo.vkInfo);
	// Present the current buffer to the swap chain
	// This will display the image
	context.swapchain->queuePresent(*context.queue, currentBuffer);

	// Add a post present image memory barrier
	// This will transform the frame buffer color attachment back
	// to it's initial layout after it has been presented to the
	// windowing system
	// See buildCommandBuffers for the pre present barrier that 
	// does the opposite transformation 
	// Use dedicated command buffer from example base class for submitting the post present barrier

	VulkanImageMemoryBarrier postPresentBarrier;
	postPresentBarrier
		.setSrcAccessMask(0)
		.setDstAccessMask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
		.setOldLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.setNewLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setSubresourceRange({ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 })
		.setImage(context.swapchain->buffers[currentBuffer].image);

	VulkanCommandBufferBeginInfo cmdBufInfo;
	context.postPresentCmdBuffer
		.beginCommandBuffer(cmdBufInfo.vkInfo)
		.putPipelineBarrier(postPresentBarrier.vkBarrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
		.end();

	// Submit to the queue
	std::vector<VkCommandBuffer> buffers{ context.postPresentCmdBuffer.vkBuffer };
	submitInfo.setCommandBuffers(buffers);

	context.queue->submit(1, submitInfo.vkInfo);
	context.queue->waitIdle();

	context.device->waitIdle();
}

/*
* Initializes the vertices and uploads them to the GPU. 
*/
void VulkanTriangle::prepareVertices()
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

	VulkanMemoryAllocateInfo memAlloc;
	memAlloc
		.setAllocationSize(0)
		.setMemoryTypeIndex(0);

	VkMemoryRequirements memReqs;
	void *data;

	// Generate vertex buffer
	//	Setup
	VulkanBufferCreateInfo bufInfo;
	bufInfo
		.setSize(vertexBufferSize)
		.setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
		.setFlags(0);

	//	Copy vertex data to VRAM
	memset(&vertices, 0, sizeof(vertices));
	vertices.buf = context.device->createBuffer(bufInfo.vkInfo);
	memReqs = context.device->getBufferMemoryRequirements(vertices.buf);
	memAlloc.vkInfo.allocationSize = memReqs.size;

	for (uint32_t i = 0; i < 32; i++)
	{
		if ((memReqs.memoryTypeBits & 1) == 1)
		{
			if ((context.physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				memAlloc.vkInfo.memoryTypeIndex = i;
			}
		}
		memReqs.memoryTypeBits >>= 1;
	}

	memReqs.memoryTypeBits = 1;
	vertices.mem = context.device->allocateMemory(memAlloc.vkInfo);
	data = context.device->mapMemory(vertices.mem, memAlloc.vkInfo.allocationSize);
	memcpy(data, vertexBuffer.data(), vertexBufferSize);
	context.device->unmapMemory(vertices.mem);
	context.device->bindBufferMemory(vertices.buf, vertices.mem);

	// Generate index buffer
	//	Setup
	VulkanBufferCreateInfo indexBufferInfo;
	indexBufferInfo
		.setSize(indexBufferSize)
		.setUsage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
		.setFlags(0);

	// Copy index data to VRAM
	memset(&indices, 0, sizeof(indices));
	indices.buf = context.device->createBuffer(bufInfo.vkInfo);
	memReqs = context.device->getBufferMemoryRequirements(indices.buf);

	memAlloc.vkInfo.allocationSize = memReqs.size;

	for (uint32_t i = 0; i < 32; i++)
	{
		if ((memReqs.memoryTypeBits & 1) == 1)
		{
			if ((context.physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				memAlloc.vkInfo.memoryTypeIndex = i;
			}
		}
		memReqs.memoryTypeBits >>= 1;
	}

	indices.mem = context.device->allocateMemory(memAlloc.vkInfo);
	data = context.device->mapMemory(indices.mem, indexBufferSize);
	memcpy(data, indexBuffer.data(), indexBufferSize);
	context.device->unmapMemory(indices.mem);
	context.device->bindBufferMemory(indices.buf, indices.mem);
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
void VulkanTriangle::prepareUniformBuffers()
{
	VkMemoryRequirements memReqs;

	// Vertex shader uniform buffer block
	VulkanBufferCreateInfo bufferInfo;
	bufferInfo
		.setSize(sizeof(uboVS))
		.setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

	VulkanMemoryAllocateInfo allocInfo;
	allocInfo
		.setAllocationSize(0)
		.setMemoryTypeIndex(0);

	// Create a new buffer
	uniformDataVS.buffer = context.device->createBuffer(bufferInfo.vkInfo);
	// Get memory requirements including size, alignment and memory type 
	memReqs = context.device->getBufferMemoryRequirements(uniformDataVS.buffer);
	allocInfo.vkInfo.allocationSize = memReqs.size;
	// Gets the appropriate memory type for this type of buffer allocation
	// Only memory types that are visible to the host

	for (uint32_t i = 0; i < 32; i++)
	{
		if ((memReqs.memoryTypeBits & 1) == 1)
		{
			if ((context.physicalDevice->memoryProperties().memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
				allocInfo.vkInfo.memoryTypeIndex = i;
		}
		memReqs.memoryTypeBits >>= 1;
	}

	// Allocate memory for the uniform buffer
	uniformDataVS.memory = context.device->allocateMemory(allocInfo.vkInfo);
	// Bind memory to buffer
	context.device->bindBufferMemory(uniformDataVS.buffer, uniformDataVS.memory);

	// Store information in the uniform's descriptor
	uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
	uniformDataVS.descriptor.offset = 0;
	uniformDataVS.descriptor.range = sizeof(uboVS);

	// Update matrices
	float zoom = -2.5f;
	glm::vec3 rotation;

	uboVS.projectionMatrix = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 256.0f);
	uboVS.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, zoom));

	uboVS.modelMatrix = glm::mat4();
	uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Map uniform buffer and update it
	uint8_t *pData;
	pData = (uint8_t*) context.device->mapMemory(uniformDataVS.memory, sizeof(uboVS));
	memcpy(pData, &uboVS, sizeof(uboVS));
	context.device->unmapMemory(uniformDataVS.memory);
}

/*
* Initializes the Descriptor Set Layout, used to connect shader stages to uniform buffers, image samplers etc. Each shader binding should map to one descriptor set layout.
*/
void VulkanTriangle::prepareDescriptorSetLayout()
{
	// Binding 0 : Uniform buffer (Vertex shader)
	VulkanDescriptorSetLayoutBinding layoutBinding;
	layoutBinding
		.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		.setDescriptorCount(1)
		.setStageFlags(VK_SHADER_STAGE_VERTEX_BIT)
		.setImmutableSamples(NULL);

	VulkanDescriptorSetLayoutCreateInfo descriptorLayout;
	descriptorLayout
		.setBindingCount(1)
		.setBindings(&layoutBinding.vkLayoutBinding);

	descriptorSetLayout = context.device->createDescriptorSetLayout(descriptorLayout.vkInfo);

	// Create the pipeline layout that is used to generate the rendering pipelines that
	// are based on this descriptor set layout
	// In a more complex scenario you would have different pipeline layouts for different
	// descriptor set layouts that could be reused
	VulkanPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo
		.setSetLayoutCount(1)
		.setSetLayouts(&descriptorSetLayout);

	pipelineLayout = context.device->createPipelineLayout(pipelineLayoutCreateInfo.vkInfo);
}

/*
* Initializes and creates the pipeline.
*/
void VulkanTriangle::preparePipeline()
{
	// Vulkan uses the concept of rendering pipelines to encapsulate fixed states
	
	VulkanGraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo
		.setLayout(pipelineLayout)
		.setRenderPass(context.renderPass);

	// Vertex input state
	// Describes the topoloy used with this pipeline
	VulkanPipelineInputAssemblyStateCreateInfo inputAssemblyState;
	inputAssemblyState
		.setTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	// Rasterization state
	VulkanPipelineRasterizationStateCreateInfo rasterizationState;
	rasterizationState
		.setPolygonMode(VK_POLYGON_MODE_FILL)
		.setCullMode(VK_CULL_MODE_NONE)
		.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE)
		.setDepthClampEnable(VK_FALSE)
		.setRasterizerDiscardEnable(VK_FALSE)
		.setDepthBiasEnable(VK_FALSE);

	// Color blend state
	// Describes blend modes and color masks
	std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentState(1);
	blendAttachmentState[0] = VulkanPipelineColorBlendAttachmentState()
		.setColorWriteMask(0xf)
		.setBlendEnable(VK_FALSE)
		.vkState;

	// One blend attachment state
	// Blending is not used in this example
	VulkanPipelineColorBlendStateCreateInfo colorBlendState;
	colorBlendState
		.setAttachments(blendAttachmentState);

	// Viewport state
	VulkanPipelineViewportStateCreateInfo viewportState;
	viewportState
		.setViewportCount(1)
		.setScissorCount(1);

	// Enable dynamic states
	// Describes the dynamic states to be used with this pipeline
	// Dynamic states can be set even after the pipeline has been created
	// So there is no need to create new pipelines just for changing
	// a viewport's dimensions or a scissor box
	std::vector<VkDynamicState> states{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VulkanPipelineDynamicStateCreateInfo dynamicState;
	dynamicState.setDynamicStates(states);

	// Depth and stencil state
	// Describes depth and stenctil test and compare ops
	// Basic depth compare setup with depth writes and depth test enabled
	// No stencil used 
	VulkanPipelineDepthStencilStateCreateInfo depthStencilState;
	depthStencilState
		.setDepthTestEnable(VK_TRUE)
		.setDepthWriteEnable(VK_TRUE)
		.setDepthCompareOp(VK_COMPARE_OP_LESS_OR_EQUAL)
		.setDepthBoundsTestEnable(VK_FALSE)
		.setFailOp(VK_STENCIL_OP_KEEP)
		.setPassOp(VK_STENCIL_OP_KEEP)
		.setCompareOp(VK_COMPARE_OP_ALWAYS)
		.setStencilTestEnable(VK_FALSE)
		.setFront(depthStencilState.vkInfo.back);

	// Multi sampling state
	VulkanPipelineMultisampleStateCreateInfo multisampleState;
	multisampleState
		.setSampleMask(NULL)
		.setRasterizationSamples(VK_SAMPLE_COUNT_1_BIT);

	// Load shaders
	std::string entryPoint = "main";


	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);
	shaderStages[0] = VulkanPipelineShaderStageCreateInfo()
		.setStage(VK_SHADER_STAGE_VERTEX_BIT)
		.setModule(loadShader(VERTEX_LOCATION.c_str(), context.device->vkDevice, VK_SHADER_STAGE_VERTEX_BIT))
		.setName(entryPoint)
		.vkInfo;
	assert(shaderStages[0].module != NULL);

	shaderStages[1] = VulkanPipelineShaderStageCreateInfo()
		.setStage(VK_SHADER_STAGE_FRAGMENT_BIT)
		.setModule(loadShader(FRAGMENT_LOCATION.c_str(), context.device->vkDevice, VK_SHADER_STAGE_FRAGMENT_BIT))
		.setName(entryPoint)
		.vkInfo;
	assert(shaderStages[1].module != NULL);

	// Assign states
	// Two shader stages
	pipelineCreateInfo
		.setVertexInputState(vertices.vi)
		.setInputAssemblyState(inputAssemblyState.vkInfo)
		.setRasterizationState(rasterizationState.vkInfo)
		.setColorBlendState(colorBlendState.vkInfo)
		.setViewportState(viewportState.vkInfo)
		.setDynamicState(dynamicState.vkInfo)
		.setDepthStencilState(depthStencilState.vkInfo)
		.setMultisampleState(multisampleState.vkInfo)
		.setStages(shaderStages)
		.setRenderPass(context.renderPass);

	// Create rendering pipeline
	pipelines.solid = context.device->createGraphicsPipelines(context.pipelineCache, pipelineCreateInfo.vkInfo, 1).at(0);
}

/*
* Initializes the Descriptor Pool.
*/
void VulkanTriangle::prepareDescriptorPool()
{
	// This example only uses one descriptor type (uniform buffer) and only
	// requests one descriptor of this type
	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// typeCounts[1].descriptorCount = 2;

	// We need to tell the API the number of max. requested descriptors per type
	VulkanDescriptorPoolSize typeCounts;
	typeCounts.setType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER).setDescriptorCount(1);

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	VulkanDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo
		.setPoolSizeCount(1)
		.setPoolSizes(&typeCounts.vkPoolSize)
		// Set the max. number of sets that can be requested
		// Requesting descriptors beyond maxSets will result in an error
		.setMaxSets(1);

	descriptorPool = context.device->createDescriptorPool(descriptorPoolInfo.vkInfo);
}

void VulkanTriangle::updateDescriptorSet()
{
	// Update descriptor sets determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point

	VulkanDescriptorSetAllocInfo allocInfo;
	allocInfo
		.setDescriptorPool(descriptorPool)
		.setDescriptorSetCount(1)
		.setSetLayouts(descriptorSetLayout);
	context.device->allocateDescriptorSet(allocInfo.vkInfo, descriptorSet);

	// Binding 0 : Uniform buffer
	VulkanWriteDescriptorSet writeDescriptorSet;
	writeDescriptorSet
		.setDstSet(descriptorSet)
		.setDescriptorCount(1)
		.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		.setBufferInfo(&uniformDataVS.descriptor)
		// Binds this uniform buffer to binding point 0
		.setDstBinding(0);
	context.device->updateDescriptorSet(writeDescriptorSet.vkDescriptorSet);
}

void VulkanTriangle::prepareCommandBuffers()
{
	VulkanCommandBufferBeginInfo cmdBufInfo;

	std::vector<VkClearValue> clearValues(2);
	clearValues.at(0) = VulkanClearValue()
		.setColor({ { 0.025f, 0.025f, 0.025f, 1.0f } }).vkValue;
	clearValues.at(1) = VulkanClearValue()
		.setDepthStencil({ 1.0f, 0 }).vkValue;

	// TODO create rectangle constructor to make this a single line
	VulkanRectangle2D renderArea;
	renderArea.setX(0).setY(0).setWidth(width).setHeight(height);
	VulkanRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo
		.setRenderPass(context.renderPass)
		.setRenderArea(renderArea.vkRectangle)
		.setClearValues(clearValues);

	for (int32_t i = 0; i < context.drawCmdBuffers.size(); ++i)
	{
		// Set target frame buffer
		renderPassBeginInfo.setFramebuffer(context.frameBuffers.at(i));

		// Vulkan object preparation

		// Update dynamic viewport state
		VulkanViewport viewport;
		viewport
			.setHeight(static_cast<float>(height)).setWidth(static_cast<float>(width))
			.setMinDepth(0.0f).setMaxDepth(1.0f);

		// Update dynamic scissor state
		VulkanRectangle2D scissor;
		scissor.setWidth(width).setHeight(height).setX(0).setY(0);
		
		// Add a present memory barrier to the end of the command buffer
		// This will transform the frame buffer color attachment to a
		// new layout for presenting it to the windowing system integration 
		VulkanImageSubresourceRange subresourceRange;
		subresourceRange
			.setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
			.setBaseMipLevel(0).setLevelCount(1).setBaseArrayLayer(0).setLayerCount(1);

		VulkanImageMemoryBarrier prePresentBarrier;
		prePresentBarrier
			.setSrcAccessMask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
			.setDstAccessMask(VK_ACCESS_MEMORY_READ_BIT)
			.setOldLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.setNewLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(context.swapchain->buffers[i].image)
			.setSubresourceRange(subresourceRange.vkRange);

		// Command chaining
		VulkanCommandBuffer& buffer = context.drawCmdBuffers.at(i);
		buffer
			.beginCommandBuffer(cmdBufInfo.vkInfo)
			.beginRenderPass(renderPassBeginInfo.vkInfo, VK_SUBPASS_CONTENTS_INLINE)
			.setViewport(viewport.vkViewport)
			.setScissors(scissor.vkRectangle)
			// Bind descriptor sets describing shader binding points
			.bindDescriptorSet(pipelineLayout, descriptorSet)
			// Bind the pipeline with shaders, vertex and index buffers, and draw indexed triangles
			.bindPipeline(pipelines.solid)
			.bindVertexBuffers(vertices.buf)
			.bindIndexBuffers(indices.buf)
			.drawIndexed(indices.count)
			.endRenderPass()
			.putPipelineBarrier(prePresentBarrier.vkBarrier, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
			.end();
	}
}