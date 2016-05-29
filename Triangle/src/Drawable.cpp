#include "Drawable.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Shaders.h"
#include "ObjImporter.h"

#include "VulkanWrappers.h"


Drawable::Drawable(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkRenderPass& renderPass, VkPipelineCache& pipelineCache, std::vector<VkFramebuffer>& frameBuffers, const VulkanSwapChain& swapchain, VkCommandPool& cmdPool, VulkanQueue& queue, Camera& camera)
{
	prepareTexture(physicalDevice, device, cmdPool, queue);
	prepareVertices(device, physicalDevice);
	prepareUniformBuffers(camera, device, physicalDevice);
	prepareDescriptorSetLayout(device);
	preparePipeline(renderPass, device, pipelineCache);
	prepareDescriptorPool(device);
	updateDescriptorSet(device);
	prepareCommandBuffers(camera, device, frameBuffers, renderPass, swapchain, cmdPool);
}

void Drawable::prepareTexture(VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, VkCommandPool& pool, VulkanQueue& queue)
{
	
	texture.load("./../data/textures/tree.dds", VK_FORMAT_BC3_UNORM_BLOCK, physicalDevice, device, pool, queue);
}

void Drawable::prepareVertices(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
{
	Mesh* m = Importers::Obj::load("./../data/models/tree.obj");

	int vertexBufferSize = m->vertices.size() * sizeof(Vertex);
	int indexBufferSize = m->indices.size() * sizeof(uint32_t);

	VulkanMemoryAllocateInfo memAlloc;
	memAlloc
		.setAllocationSize(0)
		.setMemoryTypeIndex(0);

	VkMemoryRequirements memReqs;
	void *data;

	// Generate vertex buffer
	// Setup
	VulkanBufferCreateInfo bufInfo;
	bufInfo
		.setSize(vertexBufferSize)
		.setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
		.setFlags(0);

	//	Copy data to VRAM
	memset(&vertices, 0, sizeof(vertices));
	vertices.buf = device.createBuffer(bufInfo.vkInfo);
	memReqs = device.getBufferMemoryRequirements(vertices.buf);
	memAlloc.vkInfo.allocationSize = memReqs.size;
	memAlloc.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

	memReqs.memoryTypeBits = 1;
	vertices.mem = device.allocateMemory(memAlloc.vkInfo);
	data = device.mapMemory(vertices.mem, memAlloc.vkInfo.allocationSize);
	memcpy(data, m->vertices.data(), vertexBufferSize);
	device.unmapMemory(vertices.mem);
	device.bindBufferMemory(vertices.buf, vertices.mem);

	// Generate index buffer
	//	Setup
	VulkanBufferCreateInfo indexBufferInfo;
	indexBufferInfo
		.setSize(indexBufferSize)
		.setUsage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
		.setFlags(0);

	// Copy index data to VRAM
	memset(&indices, 0, sizeof(indices));
	indices.buf = device.createBuffer(bufInfo.vkInfo);
	memReqs = device.getBufferMemoryRequirements(indices.buf);

	memAlloc.vkInfo.allocationSize = memReqs.size;
	memAlloc.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

	indices.mem = device.allocateMemory(memAlloc.vkInfo);
	data = device.mapMemory(indices.mem, indexBufferSize);
	memcpy(data, m->indices.data(), indexBufferSize);
	device.unmapMemory(indices.mem);
	device.bindBufferMemory(indices.buf, indices.mem);
	indices.count = m->indices.size();

	// Binding description
	vertices.bindingDescriptions.resize(1);
	vertices.bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	vertices.bindingDescriptions[0].stride = sizeof(Vertex);
	vertices.bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// Attribute descriptions
	// Describes memory layout and shader attribute locations
	vertices.attributeDescriptions.resize(3);
	// Location 0 : Position
	vertices.attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	vertices.attributeDescriptions[0].location = 0;
	vertices.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertices.attributeDescriptions[0].offset = 0;
	// Location 1 : UV
	vertices.attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
	vertices.attributeDescriptions[1].location = 1;
	vertices.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertices.attributeDescriptions[1].offset = sizeof(float) * 3;
	// Location 2 : Normal
	vertices.attributeDescriptions[2].binding = VERTEX_BUFFER_BIND_ID;
	vertices.attributeDescriptions[2].location = 2;
	vertices.attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertices.attributeDescriptions[2].offset = sizeof(float) * 5;

	// Assign to vertex buffer
	vertices.vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertices.vi.pNext = NULL;
	vertices.vi.vertexBindingDescriptionCount = vertices.bindingDescriptions.size();
	vertices.vi.pVertexBindingDescriptions = vertices.bindingDescriptions.data();
	vertices.vi.vertexAttributeDescriptionCount = vertices.attributeDescriptions.size();
	vertices.vi.pVertexAttributeDescriptions = vertices.attributeDescriptions.data();
}

void Drawable::prepareUniformBuffers(Camera& camera, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice)
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
	uniformDataVS.buffer = device.createBuffer(bufferInfo.vkInfo);
	// Get memory requirements including size, alignment and memory type 
	memReqs = device.getBufferMemoryRequirements(uniformDataVS.buffer);
	allocInfo.vkInfo.allocationSize = memReqs.size;
	// Gets the appropriate memory type for this type of buffer allocation
	// Only memory types that are visible to the host

	allocInfo.setMemoryTypeIndex(physicalDevice.getMemoryPropertyIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memReqs));

	// Allocate memory for the uniform buffer
	uniformDataVS.memory = device.allocateMemory(allocInfo.vkInfo);
	// Bind memory to buffer
	device.bindBufferMemory(uniformDataVS.buffer, uniformDataVS.memory);

	// Store information in the uniform's descriptor
	uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
	uniformDataVS.descriptor.offset = 0;
	uniformDataVS.descriptor.range = sizeof(uboVS);

	updateUniformBuffers(camera, device);
}

void Drawable::prepareDescriptorSetLayout(VulkanDevice& device)
{
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings(2);
		// Binding 0 : Uniform buffer (Vertex shader)
	setLayoutBindings[0] = VulkanDescriptorSetLayoutBinding()
		.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		.setDescriptorCount(1)
		.setBinding(0)
		.setStageFlags(VK_SHADER_STAGE_VERTEX_BIT)
		.vkLayoutBinding;

		// Binding 1: Fragment shader image sampler
	setLayoutBindings[1] = VulkanDescriptorSetLayoutBinding()
		.setDescriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		.setDescriptorCount(1)
		.setBinding(1)
		.setStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT)
		.vkLayoutBinding;

	VulkanDescriptorSetLayoutCreateInfo descriptorLayout;
	descriptorLayout
		.setBindings(setLayoutBindings);

	descriptorSetLayout = device.createDescriptorSetLayout(descriptorLayout.vkInfo);

	// Create the pipeline layout that is used to generate the rendering pipelines that
	// are based on this descriptor set layout
	// In a more complex scenario you would have different pipeline layouts for different
	// descriptor set layouts that could be reused
	VulkanPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo
		.setSetLayoutCount(1)
		.setSetLayouts(&descriptorSetLayout);

	pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo.vkInfo);
}

void Drawable::preparePipeline(VkRenderPass& renderPass, VulkanDevice& device, VkPipelineCache& pipelineCache)
{

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
		.setModule(loadShader(VERTEX_LOCATION.c_str(), device.vkDevice, VK_SHADER_STAGE_VERTEX_BIT))
		.setName(entryPoint)
		.vkInfo;
	assert(shaderStages[0].module != NULL);

	shaderStages[1] = VulkanPipelineShaderStageCreateInfo()
		.setStage(VK_SHADER_STAGE_FRAGMENT_BIT)
		.setModule(loadShader(FRAGMENT_LOCATION.c_str(), device.vkDevice, VK_SHADER_STAGE_FRAGMENT_BIT))
		.setName(entryPoint)
		.vkInfo;
	assert(shaderStages[1].module != NULL);

	// Assign states
	// Two shader stages
	// Vulkan uses the concept of rendering pipelines to encapsulate fixed states
	VulkanGraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo
		.setLayout(pipelineLayout)
		.setVertexInputState(vertices.vi)
		.setInputAssemblyState(inputAssemblyState.vkInfo)
		.setRasterizationState(rasterizationState.vkInfo)
		.setColorBlendState(colorBlendState.vkInfo)
		.setViewportState(viewportState.vkInfo)
		.setDynamicState(dynamicState.vkInfo)
		.setDepthStencilState(depthStencilState.vkInfo)
		.setMultisampleState(multisampleState.vkInfo)
		.setStages(shaderStages)
		.setRenderPass(renderPass);

	// Create rendering pipeline
	pipelines.solid = device.createGraphicsPipelines(pipelineCache, pipelineCreateInfo.vkInfo, 1).at(0);
}

void Drawable::prepareDescriptorPool(VulkanDevice& device)
{
	// We need to tell the API the number of max. requested descriptors per type
	VulkanDescriptorPoolSize typeCounts;
	std::vector<VkDescriptorPoolSize> poolSize
	{
		VulkanDescriptorPoolSize().setType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER).setDescriptorCount(1).vkPoolSize,
		VulkanDescriptorPoolSize().setType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER).setDescriptorCount(1).vkPoolSize
	};

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	VulkanDescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo
		.setPoolSizes(poolSize)
		// Set the max. number of sets that can be requested
		// Requesting descriptors beyond maxSets will result in an error
		.setMaxSets(2);

	descriptorPool = device.createDescriptorPool(descriptorPoolInfo.vkInfo);
}

void Drawable::updateDescriptorSet(VulkanDevice& device)
{
	// Update descriptor sets determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point

	VulkanDescriptorSetAllocInfo allocInfo;
	allocInfo
		.setDescriptorPool(descriptorPool)
		.setDescriptorSetCount(1)
		.setSetLayouts(descriptorSetLayout);
	device.allocateDescriptorSet(allocInfo.vkInfo, descriptorSet);

	VkDescriptorImageInfo texDescriptor;
	texDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	texDescriptor.sampler = texture.sampler;
	texDescriptor.imageView = texture.view;

	std::vector<VkWriteDescriptorSet> descriptorSets(2);
		// Binding 0 : Uniform buffer
	descriptorSets[0] = VulkanWriteDescriptorSet()
		.setDstSet(descriptorSet)
		.setDescriptorCount(1)
		.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		.setBufferInfo(&uniformDataVS.descriptor)
		.setDstBinding(0)
		.vkDescriptorSet;

	descriptorSets[1] = VulkanWriteDescriptorSet()
		.setDstSet(descriptorSet)
		.setDescriptorCount(1)
		.setDescriptorType(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		.setImageInfo(&texDescriptor)
		.setDstBinding(1)
		.vkDescriptorSet;

	device.updateDescriptorSet(descriptorSets);
}

void Drawable::prepareCommandBuffers(Camera& camera, VulkanDevice& device, std::vector<VkFramebuffer>& frameBuffers, const VkRenderPass& renderPass, const VulkanSwapChain& swapchain, VkCommandPool& cmdPool)
{
	VulkanCommandBufferAllocateInfo info;
	info.setCommandPool(cmdPool)
		.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
		.setCommandBufferCount(swapchain.imageCount);

	commandBuffers = device.allocateCommandBuffers(info.vkInfo);

	VulkanCommandBufferBeginInfo cmdBufInfo;

	std::vector<VkClearValue> clearValues(2);
	clearValues.at(0) = VulkanClearValue()
		.setColor({ { 0.0f, 0.0f, 0.5f, 1.0f } }).vkValue;
	clearValues.at(1) = VulkanClearValue()
		.setDepthStencil({ 1.0f, 0 }).vkValue;

	// TODO create rectangle constructor to make this a single line
	VulkanRectangle2D renderArea;
	renderArea
		.setX(0)
		.setY(0)
		.setWidth(camera.dimensions.x)
		.setHeight(camera.dimensions.y);
	
	VulkanRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo
		.setRenderPass(renderPass)
		.setRenderArea(renderArea.vkRectangle)
		.setClearValues(clearValues);

	for (int32_t i = 0; i < commandBuffers.size(); ++i)
	{
		// Set target frame buffer
		renderPassBeginInfo.setFramebuffer(frameBuffers.at(i));

		// Vulkan object preparation

		// Update dynamic viewport state
		VulkanViewport viewport;
		viewport
			.setWidth(camera.dimensions.x).setHeight(camera.dimensions.y)
			.setMinDepth(0.0f).setMaxDepth(1.0f);

		// Update dynamic scissor state
		VulkanRectangle2D scissor;
		scissor.setWidth(camera.dimensions.x).setHeight(camera.dimensions.y).setX(0).setY(0);
		
		// Add a present memory barrier to the end of the command buffer
		// This will transform the frame buffer color attachment to a
		// new layout for presenting it to the windowing system integration 
		VulkanImageSubresourceRange subresourceRange;
		subresourceRange
			.setAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
			.setBaseMipLevel(0).setLevelCount(1).setBaseArrayLayer(0).setLayerCount(1);

		// Command chaining
		VulkanCommandBuffer& buffer = commandBuffers.at(i);
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
			.end();
	}
}

void Drawable::updateUniformBuffers(Camera& camera, VulkanDevice& device)
{
	// Update matrices
	camera.update();
	uboVS.projectionMatrix = camera.getProjection();
	uboVS.viewMatrix = camera.getView();
	uboVS.modelMatrix = glm::mat4();
	uboVS.modelMatrix = glm::scale(uboVS.modelMatrix, glm::vec3(1, -1, 1));

	// Map uniform buffer and update it
	void *pData;
	pData = device.mapMemory(uniformDataVS.memory, sizeof(uboVS));
	memcpy(pData, &uboVS, sizeof(uboVS));
	device.unmapMemory(uniformDataVS.memory);
}

std::vector<VulkanCommandBuffer>& Drawable::getCommandBuffers()
{
	return this->commandBuffers;
}
