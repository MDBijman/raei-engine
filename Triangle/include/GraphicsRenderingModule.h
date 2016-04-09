#pragma once
#define VERTEX_BUFFER_BIND_ID 0
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <math.h>
#include <vector>
#include <chrono>
#include <memory>

#include "Shaders.h"

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

class GraphicsRenderingModule 
{
public:
	GraphicsRenderingModule(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, const glm::fvec2& SCREEN_DIMENSIONS, VkRenderPass& renderPass, VkPipelineCache& pipelineCache, std::vector<VulkanCommandBuffer>& drawCmdBuffers, std::vector<VkFramebuffer>& frameBuffers, const VulkanSwapChain& swapchain);
	
	void render(VulkanDevice& device, VulkanSwapChain& swapchain, VulkanQueue& queue, std::vector<VulkanCommandBuffer>& drawCmdBuffers, VulkanCommandBuffer& postPresentCmdBuffer, const VkSemaphore& presentComplete, const VkSemaphore& renderComplete);

private:
	const std::string SHADER_LOCATION = "C:\\Dev\\Vulkan\\data\\shaders\\";
	const std::string VERTEX_LOCATION = SHADER_LOCATION + "triangle.vert.spv";
	const std::string FRAGMENT_LOCATION = SHADER_LOCATION + "triangle.frag.spv";

	VkPipelineLayout pipelineLayout;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

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
	void prepareVertices(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);

	/*
	* Initializes the Uniform Buffers of the shader.
	*/
	void prepareUniformBuffers(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, const glm::fvec2& SCREEN_DIMENSIONS);

	/*
	* Initializes the Descriptor Set Layout, used to connect shader stages to uniform buffers, image samplers etc. Each shader binding should map to one descriptor set layout.
	*/
	void prepareDescriptorSetLayout(VulkanDevice& device);

	/*
	* Initializes and creates the pipeline.
	*/
	void preparePipeline(VkRenderPass& renderPass, VulkanDevice& device, VkPipelineCache& pipelineCache);

	/*
	* Initializes the Descriptor Pool.
	*/
	void prepareDescriptorPool(VulkanDevice& device);

	/*
	* Updates the Descriptor Set.
	*/
	void updateDescriptorSet(VulkanDevice& device);

	/*
	* Prepares the Command Buffers.
	*/
	void prepareCommandBuffers(const glm::fvec2& SCREEN_DIMENSIONS, std::vector<VulkanCommandBuffer>& drawCmdBuffers, std::vector<VkFramebuffer>& frameBuffers, const VkRenderPass& renderPass, const VulkanSwapChain& swapchain);

	/*
	* Updates the Uniform Buffers.
	*/
	void updateUniformBuffers(const glm::fvec2& SCREEN_DIMENSIONS, VulkanDevice& device);
};