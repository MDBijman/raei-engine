#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Logic/Pipeline.h"
#include <glm/glm.hpp>

namespace Components
{
	class CommandBuffers : public ecs::Component
	{
	public:
		template<class ShaderType>
		CommandBuffers(vk::CommandPool& cmdPool, VulkanSwapChain& swapchain, vk::Device& device, glm::vec2& dimensions,
			vk::RenderPass& renderPass, Graphics::Pipeline& pipeline, std::vector<vk::Framebuffer>& framebuffers,
			ShaderType& shader)
		{
			vk::CommandBufferAllocateInfo info;
			info.setCommandPool(cmdPool)
				.setLevel(vk::CommandBufferLevel::ePrimary)
				.setCommandBufferCount(static_cast<uint32_t>(swapchain.images.size()));

			commandBuffers = new std::vector<vk::CommandBuffer>(device.allocateCommandBuffers(info));

			vk::CommandBufferBeginInfo cmdBufInfo;

			vk::Rect2D renderArea;
			renderArea
				.setExtent(swapchain.swapchainExtent)
				.setOffset({ 0, 0 });

			vk::RenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo
				.setRenderPass(renderPass)
				.setRenderArea(renderArea);

			for(int32_t i = 0; i < commandBuffers->size(); ++i)
			{
				// Set target frame buffer, std::vector<VkFramebuffer>& framebuffers
				renderPassBeginInfo.setFramebuffer(framebuffers.at(i));

				// Vulkan object preparation

				// Update dynamic viewport state
				vk::Viewport viewport;
				viewport
					.setWidth(dimensions.x)
					.setHeight(dimensions.y)
					.setMinDepth(0.0f)
					.setMaxDepth(1.0f);

				// Update dynamic scissor state
				vk::Rect2D scissor;
				scissor
					.setExtent(vk::Extent2D()
						.setWidth(static_cast<uint32_t>(dimensions.x))
						.setHeight(static_cast<uint32_t>(dimensions.y)));

				// Command chaining
				vk::CommandBuffer& buffer = commandBuffers->at(i);
				buffer.begin(cmdBufInfo);

				buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
				buffer.setViewport(0, viewport);
				buffer.setScissor(0, scissor);
				buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.layout, 0, shader.getUniforms().getDescriptorSet(), nullptr);
				buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk);
				shader.draw(buffer);
				buffer.endRenderPass();

				buffer.end();
			}
		}

		CommandBuffers(CommandBuffers&& other) noexcept
		{
			this->commandBuffers = other.commandBuffers;
			other.commandBuffers = nullptr;
		}

		std::vector<vk::CommandBuffer>* commandBuffers;

	private:
	};

}