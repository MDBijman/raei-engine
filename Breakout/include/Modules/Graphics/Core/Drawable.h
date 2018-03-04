#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "Pipeline.h"

namespace speck::graphics
{
	template<class Shader>
	class drawable 
	{
		friend class resource_loader;

		Shader shader_;
		::graphics::Pipeline pipeline_;
		std::unique_ptr<std::vector<vk::CommandBuffer>> buffers_;

	public:
		drawable(drawable<Shader>&& o) :
			shader_(std::move(o.shader_)),
			pipeline_(std::move(o.pipeline_)),
			buffers_(std::move(o.buffers_))
		{}

		Shader& shader()
		{
			return shader_;
		}

		::graphics::Pipeline& pipeline()
		{
			return pipeline_;
		}

		std::vector<vk::CommandBuffer>& buffers()
		{
			return *buffers_;
		}

	private:
		drawable(Shader shader, ::graphics::Pipeline pipeline, std::vector<vk::CommandBuffer> buffers) :
			shader_(std::move(shader)),
			pipeline_(std::move(pipeline)),
			buffers_(std::make_unique<std::vector<vk::CommandBuffer>>(std::move(buffers)))
		{}
	};
}