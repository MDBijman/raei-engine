#pragma once
#include <tuple>
#include <vector>
#include "VulkanContext.h"
#include "../Shaders/Uniforms/Texture.h"
#include "../Shaders/Uniforms/UniformBuffer.h"

namespace speck::graphics
{
	class ResourceLoader
	{
		::graphics::VulkanContext& context_;

	public:
		ResourceLoader(::graphics::VulkanContext& context) : context_(context) {}

		ResourceLoader(ResourceLoader&& o) :
			context_(o.context_)
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
	};
}