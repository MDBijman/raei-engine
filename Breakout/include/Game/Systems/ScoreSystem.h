#pragma once
#include "Game/ECSConfig.h"
#include "Game/Components/score.h"
#include "Game/EventConfig.h"
#include "Modules/Fonts/FntLoader.h"

namespace systems
{
	class score_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;
		graphics::VulkanContext& context;
		fnt::file text_file;

	public:
		score_system(events::subscriber<events::collision>& sub, graphics::VulkanContext& context) : 
			context(context), subscriber(sub), text_file("./res/fonts/vcr_osd_mono.fnt")
		{}

		void update(ecs_manager& ecs) override
		{
			int count = 0;
			static bool first_change = true;
			bool change = false;
			auto bricks = ecs.filterEntities<ecs::filter<components::brick>>();

			while (this->subscriber.size() > 0)
			{
				auto collision = subscriber.pop();
				if (bricks.entities.find(collision->a) == bricks.entities.end()
					&& bricks.entities.find(collision->b) == bricks.entities.end())
				{
					continue;
				}

				change = true;
				count++;
			}

			if (!change && !first_change) return;
			first_change = false;

			{
				auto result = ecs.filterEntities<ecs::filter<components::score,
					components::drawable<sprite_shader>, Components::Scale2D>>();

				for (auto entity : result.entities)
				{
					auto& score = ecs.getComponent<components::score>(entity);
					score.count += count;

					auto& drawable = ecs.getComponent<components::drawable<sprite_shader>>(entity);
					auto& scale = ecs.getComponent<Components::Scale2D>(entity);

					auto& vert_data = drawable.shader().attributes().data();

					auto update_digit = [&](int digit_index, char character) {
						fnt::character& char_info = text_file.get_character(character);

						scale.scale.x = static_cast<float>(char_info.width) /
							static_cast<float>(char_info.height) * 0.05f;
						scale.scale.y = 0.05f;

						float left_x = static_cast<float>(char_info.x) / 256.0f;
						float right_x = static_cast<float>(char_info.x + char_info.width) / 256.0f;
						float top_y = static_cast<float>(char_info.y) / 256.0f;
						float bottom_y = static_cast<float>(char_info.y + char_info.height) / 256.0f;

						auto vert_data_index = digit_index * 4;

						vert_data.at(vert_data_index).rest.data.x = left_x;
						vert_data.at(vert_data_index).rest.data.y = top_y;

						vert_data.at(vert_data_index + 1).rest.data.x = left_x;
						vert_data.at(vert_data_index + 1).rest.data.y = bottom_y;

						vert_data.at(vert_data_index + 2).rest.data.x = right_x;
						vert_data.at(vert_data_index + 2).rest.data.y = top_y;

						vert_data.at(vert_data_index + 3).rest.data.x = right_x;
						vert_data.at(vert_data_index + 3).rest.data.y = bottom_y;
					};

					update_digit(2, '0' + (score.count % 10));
					update_digit(1, '0' + ((score.count / 10) % 10));
					update_digit(0, '0' + ((score.count / 100) % 10));
					drawable.shader().attributes().upload(context);
				}
			}
		}
	};
}
