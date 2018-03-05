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

		drawable<text> create_text(std::string content, ::graphics::Camera& camera)
		{
			return resources.create_drawable(text(
				text_font, 
				std::move(content),
				{
					resources.create_attributes<text::attributes_t>({
					}),
				resources.create_uniform<text::uniforms_t>({
					resources.create_buffer(
						camera.getMatrices().projection * camera.getMatrices().view * glm::mat4(), 0),
					resources.create_texture("./res/fonts/vcr_osd_mono.dds", vk::Format::eBc3UnormBlock, 1,
						vk::ShaderStageFlagBits::eFragment)
					})
				}
			));
		}
	};
}