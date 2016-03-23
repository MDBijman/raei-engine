#pragma once
#define VERTEX_BUFFER_BIND_ID 0
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <math.h>
#include <vector>
#include <iostream>
#include <chrono>
#include <memory>

#include "Shaders.h"
#include "VulkanBaseContext.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanImageCreateInfo.h"
#include "VulkanImageMemoryBarrier.h"
#include "VulkanCommandBufferAllocateInfo.h"
#include "VulkanMemoryAllocateInfo.h"
#include "VulkanImageSubresourceRange.h"
#include "VulkanImageViewCreateInfo.h"
#include "VulkanSubmitInfo.h"
#include "VulkanCommandPoolCreateInfo.h"
#include "VulkanCommandBufferBeginInfo.h"
#include "VulkanAttachmentReference.h"
#include "VulkanAttachmentDescription.h"
#include "VulkanSubpassDescription.h"
#include "VulkanViewport.h"
#include "VulkanRectangle2D.h"
#include "VulkanClearValue.h"
#include "VulkanRenderPassBeginInfo.h"
#include "VulkanDescriptorSetAllocInfo.h"
#include "VulkanWriteDescriptorSet.h"
#include "VulkanDescriptorPoolSize.h"
#include "VulkanDescriptorPoolCreateInfo.h"
#include "VulkanDescriptorSetLayoutBinding.h"
#include "VulkanDescriptorSetLayoutCreateInfo.h"
#include "VulkanPipelineLayoutCreateInfo.h"
#include "VulkanFramebufferCreateInfo.h"
#include "VulkanSemaphoreCreateInfo.h"
#include "VulkanGraphicsPipelineCreateInfo.h"
#include "VulkanPipelineInputAssemblyStateCreateInfo.h"
#include "VulkanPipelineRasterizationStateCreateInfo.h"
#include "VulkanPipelineColorBlendStateCreateInfo.h"
#include "VulkanPipelineColorBlendAttachmentState.h"
#include "VulkanPipelineViewportStateCreateInfo.h"
#include "VulkanPipelineDynamicStateCreateInfo.h"
#include "VulkanPipelineDepthStencilStateCreateInfo.h"
#include "VulkanPipelineMultisampleStateCreateInfo.h"
#include "VulkanPipelineShaderStageCreateInfo.h"
#include "VulkanRenderPassCreateInfo.h"
#include "VulkanPipelineCacheCreateInfo.h"
#include "VulkanBufferCreateInfo.h"

class VulkanTriangle
{
public:
	VulkanTriangle(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height);

	void render();

private:
	uint32_t width, height;
	std::string name;

	VulkanBaseContext context;

	VkPipelineLayout pipelineLayout;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	const std::string SHADER_LOCATION = "C:\\Dev\\Vulkan\\data\\shaders\\";
	const std::string VERTEX_LOCATION = SHADER_LOCATION + "triangle.vert.spv";
	const std::string FRAGMENT_LOCATION = SHADER_LOCATION + "triangle.frag.spv";

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

	uint32_t currentBuffer = 0;

	/*
	* Initializes the vertices and uploads them to the GPU. 
	*/
	void prepareVertices();

	/*
	* Initializes the Uniform Buffers of the shader.
	*/
	void prepareUniformBuffers();

	/*
	* Initializes the Descriptor Set Layout, used to connect shader stages to uniform buffers, image samplers etc. Each shader binding should map to one descriptor set layout.
	*/
	void prepareDescriptorSetLayout();

	/*
	* Initializes and creates the pipeline.
	*/
	void preparePipeline();

	/*
	* Initializes the Descriptor Pool.
	*/
	void prepareDescriptorPool();

	void updateDescriptorSet();

	void prepareCommandBuffers();

	void updateUniformBuffers()
	{
		// Update matrices
		uboVS.projectionMatrix = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 256.0f);

		uboVS.viewMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.5f));

		uboVS.modelMatrix = glm::mat4();
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(1.f), glm::vec3(1.0f, 0.0f, 0.0f));
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(1.f), glm::vec3(0.0f, 1.0f, 0.0f));
		uboVS.modelMatrix = glm::rotate(uboVS.modelMatrix, glm::radians(1.f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Map uniform buffer and update it
		uint8_t *pData;
		pData = (uint8_t*) context.device->mapMemory(uniformDataVS.memory, sizeof(uboVS));
		memcpy(pData, &uboVS, sizeof(uboVS));
		context.device->unmapMemory(uniformDataVS.memory);
	}
};