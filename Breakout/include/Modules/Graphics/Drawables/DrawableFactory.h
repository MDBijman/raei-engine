#pragma once
#include "Text.h"
#include "../Core/Resources.h"
#include "../Core/Camera.h"

#include "Modules/Fonts/FntLoader.h"

namespace speck::graphics
{
	class drawable_factory
	{
		resource_loader& resources;
		fnt::file text_font;

	public:
		drawable_factory(resource_loader& rl) : resources(rl), text_font("./res/fonts/vcr_osd_mono.fnt")
		{

		}

		drawable_factory(const drawable_factory& o) = delete;

		text create_text(std::string content, ::graphics::Camera& camera)
		{
			auto drawable = resources.create_drawable(text_shader(
				resources.create_attributes<text_shader::attributes_t>({
					{
						{ -.5f, -.5f },
						{ 0.0f, 0.0f }
					},
					{
						{ -.5f, .5f },
						{ 0.0f, 1.0f }
					},
					{
						{ .5f, -.5f },
						{ 1.0f, 0.0f }
					},
					{
						{ .5f, .5f },
						{ 1.0f, 1.0f }
					},
					{
						{ -.5f, -.5f },
						{ 0.0f, 0.0f }
					},
					{
						{ -.5f, .5f },
						{ 0.0f, 1.0f }
					},
					{
						{ .5f, -.5f },
						{ 1.0f, 0.0f }
					},
					{
						{ .5f, .5f },
						{ 1.0f, 1.0f }
					},
					{
						{ -.5f, -.5f },
						{ 0.0f, 0.0f }
					},
					{
						{ -.5f, .5f },
						{ 0.0f, 1.0f }
					},
					{
						{ .5f, -.5f },
						{ 1.0f, 0.0f }
					},
					{
						{ .5f, .5f },
						{ 1.0f, 1.0f }
					},
					{
						{ .5f, -.5f },
						{ 1.0f, 0.0f }
					},
					{
						{ .5f, .5f },
						{ 1.0f, 1.0f }
					},
					{
						{ -.5f, -.5f },
						{ 0.0f, 0.0f }
					},
					{
						{ -.5f, .5f },
						{ 0.0f, 1.0f }
					},
					{
						{ .5f, -.5f },
						{ 1.0f, 0.0f }
					},
					{
						{ .5f, .5f },
						{ 1.0f, 1.0f }
					}
					}),
				resources.create_uniform<text_shader::uniforms_t>({
					resources.create_buffer(
						camera.getMatrices().projection * camera.getMatrices().view * glm::mat4(), 0),
					resources.create_texture("./res/fonts/vcr_osd_mono.dds", vk::Format::eBc3UnormBlock, 1,
						vk::ShaderStageFlagBits::eFragment)
					})
			));

			return text(text_font, std::move(content), std::move(drawable));
		}
	};
}