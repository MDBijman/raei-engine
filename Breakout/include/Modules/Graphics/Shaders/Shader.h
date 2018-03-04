#pragma once
#include "Modules/Graphics/Core/Buffer.h"
#include "Modules/Graphics/Shaders/Attributes/Attributes.h"
#include "Modules/Graphics/Shaders/Uniforms/Uniforms.h"
#include "Modules/Graphics/Shaders/Uniforms/Texture.h"
#include "Modules/TemplateUtils/OrderedTuple.h"

#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>

namespace graphics
{
	template<class A, class U>
	class shader;

	template<class... A, class... U>
	class shader<data::attributes<A...>, data::Uniforms<U...>>
	{
	public:
		using attributes_t = data::attributes<A...>;
		using indices_t = data::indices;
		using uniforms_t = data::Uniforms<U...>;

	private:
		attributes_t attributes_;
		std::optional<indices_t> indices_;
		uniforms_t uniforms_;

	public:
		shader(attributes_t attributes, uniforms_t uniforms) :
			shader(std::move(attributes), std::nullopt, std::move(uniforms)) {}

		shader(attributes_t attributes, indices_t indices, uniforms_t uniforms) :
			shader(std::move(attributes), std::optional<indices_t>(std::move(indices)), std::move(uniforms)) {}

		shader(shader&& o) :
			attributes_(std::move(o.attributes_)),
			indices_(std::move(o.indices_)),
			uniforms_(std::move(o.uniforms_))
		{}

		void allocate(VulkanContext& context)
		{
			uniforms_.allocate(context);
		}

		void draw(vk::CommandBuffer& cmdBuffer)
		{
			if (is_indexed())
				cmdBuffer.bindIndexBuffer(indices_.value().vk_buffer(), 0, vk::IndexType::eUint32);

			cmdBuffer.bindVertexBuffers(0, attributes_.vk_buffer(), { 0 });

			if (is_indexed())
				cmdBuffer.drawIndexed(static_cast<uint32_t>(indices_.value().count()), 1, 0, 0, 0);
			else
				cmdBuffer.draw(static_cast<uint32_t>(attributes_.count()), 1, 0, 0);
		}

		bool is_indexed()
		{
			return indices_.has_value();
		}

		std::optional<indices_t>& indices()
		{
			return indices_;
		}

		attributes_t& attributes()
		{
			return attributes_;
		}

		uniforms_t& uniforms()
		{
			return uniforms_;
		}

	private:
		shader(attributes_t attributes, std::optional<indices_t> indices, uniforms_t uniforms) :
			attributes_(std::move(attributes)),
			indices_(std::move(indices)),
			uniforms_(std::move(uniforms))
		{}
	};
}
