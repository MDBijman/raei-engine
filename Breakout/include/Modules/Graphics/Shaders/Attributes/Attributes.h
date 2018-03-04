#pragma once
#include <memory>

#include "Modules/Graphics/Core/Buffer.h"
#include "Modules/TemplateUtils/OrderedTuple.h"


namespace speck::graphics
{
	class resource_loader;
}

namespace graphics
{
	namespace data
	{
		template<class... Ts>
		class attributes : public gpu::buffer<OrderedTuple<Ts...>>
		{
		public:
			using item_t = OrderedTuple<Ts...>;

		private:
			friend class speck::graphics::resource_loader;

			static std::vector<vk::VertexInputAttributeDescription> attribute_descriptions() {
				return {
					Ts::getAttributeDescription()...
				};
			};

			static constexpr size_t item_size = sizeof(item_t);

			std::unique_ptr<std::vector<vk::VertexInputBindingDescription>> binding_desc_;
			std::unique_ptr<std::vector<vk::VertexInputAttributeDescription>> attribute_desc_;
			vk::PipelineVertexInputStateCreateInfo vi_;

		public:

			attributes(attributes&& o) :
				buffer<OrderedTuple<Ts...>>(std::move(o)),
				binding_desc_(std::move(o.binding_desc_)),
				attribute_desc_(std::move(o.attribute_desc_)),
				vi_(std::move(o.vi_))
			{}

			vk::PipelineVertexInputStateCreateInfo& vi()
			{
				return vi_;
			}

		private:
			attributes(VulkanContext& ctx, std::vector<OrderedTuple<Ts...>> data) :
				buffer<OrderedTuple<Ts...>>(ctx, vk::BufferUsageFlagBits::eVertexBuffer, std::move(data)),
				binding_desc_(std::make_unique<std::vector<vk::VertexInputBindingDescription>>()),
				attribute_desc_(std::make_unique<std::vector<vk::VertexInputAttributeDescription>>(
					attribute_descriptions()))
			{
				binding_desc_->push_back(vk::VertexInputBindingDescription()
					.setBinding(0)
					.setStride(item_size)
					.setInputRate(vk::VertexInputRate::eVertex));

				vi_
					.setVertexBindingDescriptionCount(static_cast<uint32_t>(binding_desc_->size()))
					.setPVertexBindingDescriptions(binding_desc_->data());
				vi_
					.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attribute_desc_->size()))
					.setPVertexAttributeDescriptions(attribute_desc_->data());
			}
		};

		class indices : public gpu::buffer<uint32_t>
		{
		public:
			friend class speck::graphics::resource_loader;

			indices(indices&& o) :
				buffer<uint32_t>(std::move(o)) {}

		private:
			indices(VulkanContext& ctx, std::vector<uint32_t> data) :
				buffer<uint32_t>(ctx, vk::BufferUsageFlagBits::eIndexBuffer, std::move(data)) {}
		};
	}
}