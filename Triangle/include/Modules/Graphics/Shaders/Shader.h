#pragma once
#include "Modules/Graphics/Shaders/Attributes/Attributes.h"
#include "Modules/Graphics/Shaders/Uniforms/Uniforms.h"
#include "Modules/Graphics/Shaders/Uniforms/Texture.h"
#include "Modules/TemplateUtils/OrderedTuple.h"

#include <vulkan/vulkan.hpp>

namespace graphics
{
	template<class A, class U>
	class shader_blueprint;

	template<class... A, class... U>
	class shader_blueprint<data::Attributes<A...>, data::Uniforms<U...>>
	{
	public:
		using attributes_t = data::Attributes<A...>;
		using uniforms_t = data::Uniforms<U...>;

		static std::vector<vk::VertexInputAttributeDescription> attribute_descriptions = {
			A::getAttributeDescription()...
		};
	};

	template<class A, class U>
	class shader_instance;

	template<class... A, class... U>
	class shader_instance<data::Attributes<A...>, data::Uniforms<U...>>
	{
		using blueprint_t = shader_blueprint<data::Attributes<A...>, data::Uniforms<U...>>;

	public:
		using attributes_t = typename blueprint_t::attributes_t;
		using uniforms_t = typename blueprint_t::uniforms_t;

	private:
		attributes_t attributes;
		uniforms_t uniforms;

	public:
		shader_instance(attributes_t attributes, uniforms_t uniforms) : attributes(std::move(attributes)), 
			uniforms(std::move(uniforms))
		{

		}

		void allocate(VulkanContext& context)
		{
			uniforms.allocate(context);
			attributes.allocate(context);
		}

		void draw(vk::CommandBuffer& cmdBuffer)
		{
			if (attributes.is_indexed())
				cmdBuffer.bindIndexBuffer(attributes.index_data().value().buffer, 0, vk::IndexType::eUint32);

			cmdBuffer.bindVertexBuffers(0, attributes.vertex_data().buffer, { 0 });

			if (attributes.is_indexed())
				cmdBuffer.drawIndexed(static_cast<uint32_t>(attributes.index_data().value().data.size()), 1, 0, 0, 0);
			else
				cmdBuffer.draw(static_cast<uint32_t>(attributes.vertex_data().data.size()), 1, 0, 0);
		}

		attributes_t& attribute_data()
		{
			return attributes;
		}

		uniforms_t& uniform_data()
		{
			return uniforms;
		}
	};
}
