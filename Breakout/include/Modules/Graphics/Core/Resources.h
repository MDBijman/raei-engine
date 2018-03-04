#pragma once
#include <tuple>
#include <vector>
#include "VulkanContext.h"
#include "Swapchain.h"
#include "Drawable.h"
#include "../Shaders/Uniforms/Texture.h"
#include "../Shaders/Uniforms/UniformBuffer.h"
#include "../Shaders/Attributes/Attributes.h"
#include "Pipeline.h"
#include "Modules/Importers/PipelineImporter.h"

namespace speck::graphics
{
	class resource_loader
	{
		::graphics::VulkanContext& context_;
		VulkanSwapChain& swapchain_;
		std::vector<vk::Framebuffer>& framebuffers_;
		vk::RenderPass& renderpass_;

	public:
		resource_loader(::graphics::VulkanContext& ctx, VulkanSwapChain& swp, vk::RenderPass& rp,
			std::vector<vk::Framebuffer>& fbs) : context_(ctx), swapchain_(swp), framebuffers_(fbs), renderpass_(rp)
		{}

		resource_loader(resource_loader&& o) :
			context_(o.context_),
			swapchain_(o.swapchain_),
			framebuffers_(std::move(o.framebuffers_)),
			renderpass_(o.renderpass_)
		{}

		::graphics::data::texture create_texture(std::string location, vk::Format format, int binding,
			vk::ShaderStageFlagBits flag_bits)
		{
			return ::graphics::data::texture(location, binding, flag_bits, format, context_.physicalDevice,
				context_.device, context_.cmdPool, *context_.queue);
		}

		template<class T>
		::graphics::data::buffer<T> create_buffer(T t, int binding)
		{
			return ::graphics::data::buffer<T>(std::move(t), binding, context_);
		}

		template<class T>
		T create_uniform(typename T::data_t data)
		{
			return T(context_, std::move(data));
		}

		template<class Shader>
		::graphics::Pipeline create_pipeline(std::string location, Shader& shader)
		{
			vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo
				.setPSetLayouts(&shader.uniforms().getDescriptorSetLayout())
				.setSetLayoutCount(1);
			vk::PipelineLayout layout = context_.device.createPipelineLayout(pipelineLayoutCreateInfo);

			return Importers::Pipeline::load(location, layout, shader.attributes().vi(), renderpass_,
				context_.pipeline_cache, context_.device);
		}

		template<class Shader>
		std::vector<vk::CommandBuffer> create_command_buffers(::graphics::Pipeline& pipeline, Shader& shader)
		{
			shader.allocate(context_);

			vk::CommandBufferAllocateInfo info;
			info.setCommandPool(context_.cmdPool)
				.setLevel(vk::CommandBufferLevel::ePrimary)
				.setCommandBufferCount(static_cast<uint32_t>(swapchain_.images.size()));

			auto commandBuffers = context_.device.allocateCommandBuffers(info);

			vk::CommandBufferBeginInfo cmdBufInfo;

			vk::Rect2D renderArea;
			renderArea
				.setExtent(swapchain_.swapchainExtent)
				.setOffset({ 0, 0 });

			vk::RenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo
				.setRenderPass(renderpass_)
				.setRenderArea(renderArea);

			for (int32_t i = 0; i < commandBuffers.size(); ++i)
			{
				// Set target frame buffer, std::vector<VkFramebuffer>& framebuffers
				renderPassBeginInfo.setFramebuffer(framebuffers_.at(i));

				// Vulkan object preparation

				// Update dynamic viewport state
				vk::Viewport viewport;
				viewport
					.setWidth(context_.screen_dimensions.x)
					.setHeight(context_.screen_dimensions.y)
					.setMinDepth(0.0f)
					.setMaxDepth(1.0f);

				// Update dynamic scissor state
				vk::Rect2D scissor;
				scissor
					.setExtent(vk::Extent2D()
						.setWidth(static_cast<uint32_t>(context_.screen_dimensions.x))
						.setHeight(static_cast<uint32_t>(context_.screen_dimensions.y)));

				// Command chaining
				vk::CommandBuffer& buffer = commandBuffers.at(i);
				buffer.begin(cmdBufInfo);

				buffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
				buffer.setViewport(0, viewport);
				buffer.setScissor(0, scissor);
				buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline.layout, 0,
					shader.uniforms().getDescriptorSet(), nullptr);
				buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.vk);
				shader.draw(buffer);
				buffer.endRenderPass();

				buffer.end();
			}

			return commandBuffers;
		}

		::graphics::data::indices create_indices(const std::vector<uint32_t>& data)
		{
			return ::graphics::data::indices(context_, data);
		}

		template<class T>
		T create_attributes(const std::vector<typename T::item_t>& data)
		{
			return T(context_, data);
		}

		template<class Shader>
		drawable<Shader> create_drawable(Shader shader)
		{
			shader.allocate(context_);
			auto pipeline = create_pipeline("./res/shaders/sprite-pipeline.json", shader);
			auto command_buffers = create_command_buffers(pipeline, shader);

			return drawable<Shader>(std::move(shader), std::move(pipeline), std::move(command_buffers));
		}
	};
}