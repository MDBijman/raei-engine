#pragma once
#include "Modules/Graphics/VulkanWrappers/VulkanWrappers.h"
#include "Modules/ECS/Component.h"
#include <glm/glm.hpp>

namespace Components
{
	class CommandBuffers : public Component
	{
	public:
		CommandBuffers(VkCommandPool& cmdPool, VulkanSwapChain& swapchain, VulkanDevice& device, Camera& camera, VulkanRenderPass& renderPass, Graphics::Pipeline& pipeline, std::vector<VkFramebuffer>& framebuffers, Graphics::Data::Shader& shader, Graphics::Data::Mesh& mesh)
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
				.setWidth(static_cast<uint32_t>(camera.getDimensions().x))
				.setHeight(static_cast<uint32_t>(camera.getDimensions().y));

			VulkanRenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo
				.setRenderPass(renderPass)
				.setRenderArea(renderArea.vkRectangle)
				.setClearValues(clearValues);

			for(int32_t i = 0; i < commandBuffers.size(); ++i)
			{
				// Set target frame buffer, std::vector<VkFramebuffer>& framebuffers
				renderPassBeginInfo.setFramebuffer(framebuffers.at(i));

				// Vulkan object preparation

				// Update dynamic viewport state
				VulkanViewport viewport;
				viewport
					.setWidth(camera.getDimensions().x)
					.setHeight(camera.getDimensions().y)
					.setMinDepth(0.0f)
					.setMaxDepth(1.0f);

				// Update dynamic scissor state
				VulkanRectangle2D scissor;
				scissor
					.setWidth(static_cast<uint32_t>(camera.getDimensions().x))
					.setHeight(static_cast<uint32_t>(camera.getDimensions().y))
					.setX(0)
					.setY(0);

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
					.bindDescriptorSet(pipeline.layout.vk, shader.descriptorSet)
					// Bind the pipeline with shaders, vertex and index buffers, and draw indexed triangles
					.bindPipeline(pipeline.vk)
					.bindVertexBuffers(mesh.vertices.getBuffer())
					.bindIndexBuffers(mesh.indices.getBuffer())
					.drawIndexed(mesh.indices.count)
					.endRenderPass()
					.end();

			}

		};


		std::vector<VulkanCommandBuffer> commandBuffers;
	};

}