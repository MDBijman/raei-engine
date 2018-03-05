#pragma once
#include "../Core/Drawable.h"
#include "../Shaders/Shader.h"
#include "../Shaders/Attributes/Attributes.h"
#include "../Shaders/Attributes/Vec2.h"
#include "../Shaders/Uniforms/Uniforms.h"
#include "../Shaders/Uniforms/Texture.h"
#include "../Shaders/Uniforms/UniformBuffer.h"

#include "Modules/Fonts/FntLoader.h"

namespace speck::graphics
{
	namespace detail
	{
		using text_shader_attributes = ::graphics::data::attributes<
			::graphics::data::Vec2<0, 0>,
			::graphics::data::Vec2<1, sizeof(float) * 2>
		>;

		using text_shader_indices = typename ::graphics::data::indices;

		using text_shader_uniforms = ::graphics::data::Uniforms<
			::graphics::data::buffer_template<glm::mat4, 0, vk::ShaderStageFlagBits::eVertex>,
			::graphics::data::texture_template<1, vk::ShaderStageFlagBits::eFragment>
		>;

		using text_shader = ::graphics::shader<detail::text_shader_attributes, detail::text_shader_uniforms>;
	}

	class text : public detail::text_shader
	{
		friend class drawable_factory;
		
		std::string content;
		const fnt::file& font;

	public:
		text(text&& o) : content(std::move(o.content)), font(o.font), detail::text_shader(std::move(o)) {}

		void set_content(std::string s)
		{
			this->content = s;

			size_t length = content.size();

			std::vector<attributes_t::item_t> data;

			auto create_character = [&](char character) {
				const fnt::character& char_info = font.get_character(character);

				float left_x = static_cast<float>(char_info.x) / 256.0f;
				float right_x = static_cast<float>(char_info.x + char_info.width) / 256.0f;
				float top_y = static_cast<float>(char_info.y) / 256.0f;
				float bottom_y = static_cast<float>(char_info.y + char_info.height) / 256.0f;

				auto data_size = static_cast<float>(data.size()) / 6.0f;

				// Left tri
				data.push_back({
					{ data_size, 0.0f },
					{ left_x, top_y }
					});

				data.push_back({
					{ data_size, 1.0f },
					{ left_x, bottom_y }
					});

				data.push_back({
					{ data_size + 1.0f, 0.0f },
					{ right_x, top_y }
					});

				// Right tri
				data.push_back({
					{ data_size + 1.0f, 0.0f },
					{ right_x, top_y }
					});

				data.push_back({
					{ data_size, 1.0f },
					{ left_x, bottom_y }
					});

				data.push_back({
					{ data_size + 1.0f, 1.0f },
					{ right_x, bottom_y }
					});
			};

			for (auto character : content)
			{
				create_character(character);
			}

			this->attributes().set(std::move(data));
		}

		const std::string& get_content()
		{
			return this->content;
		}

	private:
		text(fnt::file& f, std::string txt, detail::text_shader&& ts) :
			font(f), content(txt), detail::text_shader(std::move(ts)) {}
	};
}