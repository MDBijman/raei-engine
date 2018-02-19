#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Modules/ECS/Component.h"
#include "Modules/Graphics/Graphics.h"

namespace components
{
	template<class ShaderType>
	class Drawable : public ecs::Component
	{
		ShaderType shader_;
		graphics::Pipeline pipeline_;
		std::unique_ptr<std::vector<vk::CommandBuffer>> buffers_;

	public:
		Drawable(ShaderType shader, graphics::Pipeline pipeline, std::vector<vk::CommandBuffer> bufs) :
			shader_(std::move(shader)),
			pipeline_(std::move(pipeline)),
			buffers_(std::make_unique<std::vector<vk::CommandBuffer>>(std::move(bufs))
		{
		}
	};
}