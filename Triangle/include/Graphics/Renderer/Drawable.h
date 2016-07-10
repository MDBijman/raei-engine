#pragma once
#include "Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Graphics\Renderer\Camera.h"
#include "Graphics\Renderer\Pipeline.h"
#include "Graphics\Resources\Textures\Texture.h"
#include "Graphics\Resources\Shaders\UniformData.h"
#include "Geometry\Vertices.h"
#include "Geometry\Indices.h"

#include <vulkan\vulkan.h>
#include <glm\glm.hpp>

#define VERTEX_BUFFER_BIND_ID 0

namespace Graphics
{
	class Drawable
	{
	public:
		Drawable(Camera& camera, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VulkanQueue& queue, VulkanRenderPass& renderpass, VkPipelineCache& cache, std::vector<VkFramebuffer>& framebuffers, VulkanSwapChain& swapchain);

		/*
		* Initialization Methods
		*/

		void addTexture(const std::string& name, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice, VkCommandPool& cmdPool, VulkanQueue& queue);
		void addObject(const std::string& name, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);
		void prepareUniformBuffers(Camera& camera, VulkanDevice& device, VulkanPhysicalDevice& physicalDevice);
		void prepareDescriptorSetLayout(VulkanDevice& device);
		void loadPipeline(const std::string& location, VulkanRenderPass& renderpass, VkPipelineCache& cache, VulkanDevice& device);
		void prepareDescriptorPool(VulkanDevice& device);
		void updateDescriptorSet(VulkanDevice& device);
		void prepareCommandBuffers(Camera& camera, VulkanDevice& device, VkCommandPool cmdPool, VulkanSwapChain& swapchain, VulkanRenderPass& renderPass, std::vector<VkFramebuffer>& framebuffers);

		void updateUniformBuffers(Camera& camera, VulkanDevice& device);

		std::vector<VulkanCommandBuffer>& getCommandBuffers();

	private:
		Texture texture;
		Vertices vertices;
		Indices indices;
		UniformData uniformDataVS;

		struct {
			glm::mat4 projectionMatrix;
			glm::mat4 modelMatrix;
			glm::mat4 viewMatrix;
		} uboVS;

		Pipeline solidPipeline;

		VulkanPipelineLayout      pipelineLayout;
		VulkanDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet           descriptorSet;
		VkDescriptorPool          descriptorPool;

		std::vector<VulkanCommandBuffer> commandBuffers;
	};
}

