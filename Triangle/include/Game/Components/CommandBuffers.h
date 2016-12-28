#pragma once
#include "Modules/ECS/Component.h"
#include <glm/glm.hpp>

namespace Components
{
	class CommandBuffers : public Component
	{
	public:
		template<class ShaderType>
		CommandBuffers(vk::CommandPool& cmdPool, VulkanSwapChain& swapchain, vk::Device& device, Camera& camera, vk::RenderPass& renderPass, Graphics::Pipeline& pipeline, std::vector<vk::Framebuffer>& framebuffers, ShaderType& shader)
		{
			vk::CommandBufferAllocateInfo info;
			info.setCommandPool(cmdPool)
				.setLevel(vk::CommandBufferLevel::ePrimary)
				.setCommandBufferCount(static_cast<uint32_t>(swapchain.images.size()));

			commandBuffers = new std::vector<vk::CommandBuffer>(device.allocateCommandBuffers(info));

			vk::CommandBufferBeginInfo cmdBufInfo;

			auto clearColorValues = std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f};

			clearValues = new std::vector<vk::ClearValue>();
			clearValues->resize(2);
			clearValues->at(0) = vk::ClearValue()
				.setColor(vk::ClearColorValue(clearColorValues));
			clearValues->at(1) = vk::ClearValue()
				.setDepthStencil({ 1.0f, 0 });

			vk::Rect2D renderArea;
			renderArea
				.setExtent(swapchain.swapchainExtent);

			vk::RenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo
				.setRenderPass(renderPass)
				.setRenderArea(renderArea)
				.setClearValueCount(2)
				.setPClearValues(clearValues->data());

			for(int32_t i = 0; i < commandBuffers->size(); ++i)
			{
				// Set target frame buffer, std::vector<VkFramebuffer>& framebuffers
				renderPassBeginInfo.setFramebuffer(framebuffers.at(i));

				// Vulkan object preparation

				// Update dynamic viewport state
				vk::Viewport viewport;
				viewport
					.setWidth(camera.getDimensions().x)
					.setHeight(camera.getDimensions().y)
					.setMinDepth(0.0f)
					.setMaxDepth(1.0f);

				// Update dynamic scissor state
				vk::Rect2D scissor;
				scissor
					.setExtent(vk::Extent2D()
						.setWidth(static_cast<uint32_t>(camera.getDimensions().x))
						.setHeight(static_cast<uint32_t>(camera.getDimensions().y)));

				// Add a present memory barrier to the end of the command buffer
				// This will transform the frame buffer color attachment to a
				// new layout for presenting it to the windowing system integration 
				vk::ImageSubresourceRange subresourceRange;
				subresourceRange
					.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setBaseMipLevel(0)
					.setLevelCount(1)
					.setBaseArrayLayer(0)
					.setLayerCount(1);

				// Command chaining
				vk::CommandBuffer& buffer = commandBuffers->at(i);
				buffer.begin(cmdBufInfo);
				buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
				buffer.setViewport(0, viewport);
				buffer.setScissor(0, scissor);
				buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.layout, 0, shader.getDescriptorSet(), nullptr);
				buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk);
				shader.draw(buffer);
				//buffer.bindVertexBuffers(0, shader.getAttributes().getBuffer(), offsets);
				//buffer.bindIndexBuffer(mesh->indices.getBuffer(), 0, vk::IndexType::eUint32);
				//buffer.drawIndexed(mesh->indices.count, 1, 0, 0, 1);
				//buffer.draw(6, 1, 0, 0);
				buffer.endRenderPass();
				buffer.end();

			}

		}

		CommandBuffers(CommandBuffers&& other) noexcept
		{
			this->clearValues = other.clearValues;
			other.clearValues = nullptr;

			this->commandBuffers = other.commandBuffers;
			other.commandBuffers = nullptr;
		}

		std::vector<vk::CommandBuffer>* commandBuffers;

	private:
		std::vector<vk::ClearValue>* clearValues;
	};

}