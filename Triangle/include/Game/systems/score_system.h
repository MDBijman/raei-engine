#pragma once
#include "Game/ECSConfig.h"
#include "Game/Components/score.h"
#include "Game/EventConfig.h"
#include "Modules/Fonts/FntLoader.h"
#include "brick_event_system.h"

namespace systems
{
	class score_system : public MySystem
	{
		events::subscriber<events::brick_hit>& subscriber;
		Graphics::VulkanContext& context;
		fnt::file text_file;

	public:
		score_system(events::subscriber<events::brick_hit>& sub, Graphics::VulkanContext& context) : context(context),
			subscriber(sub), text_file("./res/fonts/vcr_osd_mono.fnt") {}

		void update(ecs_manager& ecs, double dt) override
		{
			auto&[lock, entities] = ecs.filterEntities<ecs::filter<components::score, Components::SpriteShader>>();

			for (auto entity : entities)
			{
				auto& score = ecs.getComponent<components::score>(entity);
				auto& shader = ecs.getComponent<Components::SpriteShader>(entity);
				
				auto& vert_data = shader.getAttributes().getVertices().data;
				fnt::character& char_info = text_file.get_character('6');

				float left_x   = static_cast<float>(char_info.x) / 256.0f;
				float right_x  = static_cast<float>(char_info.x + char_info.width) / 256.0f;
				float top_y	   = static_cast<float>(char_info.y) / 256.0f;
				float bottom_y = static_cast<float>(char_info.y + char_info.height) / 256.0f;

				vert_data.at(0).rest.data.x = left_x;
				vert_data.at(0).rest.data.y = top_y;

				vert_data.at(1).rest.data.x = left_x;
				vert_data.at(1).rest.data.y = bottom_y;

				vert_data.at(2).rest.data.x = right_x;
				vert_data.at(2).rest.data.y = top_y;

				vert_data.at(3).rest.data.x = right_x;
				vert_data.at(3).rest.data.y = bottom_y;
				shader.getAttributes().getVertices().upload(context);
			}
		}
	};
}
