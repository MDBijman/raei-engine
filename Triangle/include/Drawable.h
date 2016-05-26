#pragma once
#define VERTEX_BUFFER_BIND_ID 0

#include <vulkan\vulkan.h>
#include <glm\glm.hpp>
#include "Texture.h"
#include "Camera.h"

#include "VulkanWrappers.h"

class Drawable
{
public:
	Drawable(VulkanDevice & device, VulkanPhysicalDevice & physicalDevice, VkRenderPass & renderPass, VkPipelineCache & pipelineCache, std::vector<VkFramebuffer>& frameBuffers, const VulkanSwapChain & swapchain, VkCommandPool& cmdPool, VulkanQueue& queue, Camera& camera);

	/*
	* Initialization Methods
	*/

	void prepareTexture(VulkanPhysicalDevice& physicalDevice, VulkanDevice& device, VkCommandPool& pool, VulkanQueue& queue);
	void prepareVertices(VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);
	void prepareUniformBuffers(Camera& camera, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);
	void prepareDescriptorSetLayout(VulkanDevice& device);
	void preparePipeline(VkRenderPass& renderPass, VulkanDevice& device, VkPipelineCache& pipelineCache);
	void prepareDescriptorPool(VulkanDevice& device);
	void updateDescriptorSet(VulkanDevice& device);
	void prepareCommandBuffers(Camera& camera, VulkanDevice& device, std::vector<VkFramebuffer>& frameBuffers, const VkRenderPass& renderPass, const VulkanSwapChain& swapchain, VkCommandPool& cmdPool);

	void updateUniformBuffers(Camera& camera, VulkanDevice& device, glm::vec3 rotation);

	std::vector<VulkanCommandBuffer>& getCommandBuffers();

private:

	const std::string SHADER_LOCATION = "C:\\Users\\matth\\Dev\\Vulkan\\data\\shaders\\";
	const std::string VERTEX_LOCATION = SHADER_LOCATION + "triangle.vert.spv";
	const std::string FRAGMENT_LOCATION = SHADER_LOCATION + "triangle.frag.spv";
	
	Texture texture;

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

	VkPipelineLayout      pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet       descriptorSet;
	VkDescriptorPool      descriptorPool;

	std::vector<VulkanCommandBuffer> commandBuffers;
};
