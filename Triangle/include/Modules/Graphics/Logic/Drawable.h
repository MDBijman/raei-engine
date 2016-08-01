#pragma once
#include "Modules\Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Modules\Graphics\Logic\Camera.h"
#include "Modules\Graphics\Logic\Pipeline.h"
#include "Modules\Graphics\Data\Geometry\Vertices.h"
#include "Modules\Graphics\Data\Geometry\Indices.h"
#include "Modules\Graphics\Data\Textures\Texture.h"
#include "Modules\Graphics\Data\Shaders\Shader.h"
#include "Modules\Graphics\Data\Geometry\Mesh.h"

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
		Data::Texture    texture;
		Data::Mesh       mesh;
		Data::MeshShader shader;

		Pipeline solidPipeline;


		std::vector<VulkanCommandBuffer> commandBuffers;
	};
}

