#pragma once
#include "Game/ECSConfig.h"
#include "Game/Components/score.h"
#include "Game/EventConfig.h"
#include "Modules/Fonts/FntLoader.h"
#include "Modules/Graphics/Drawables/Text.h"

namespace systems
{
	class score_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;

	public:
		score_system(events::subscriber<events::collision>& sub) : 
			 subscriber(sub)
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
				glm::mat4 pv;
				{
					auto result = ecs.filterEntities<ecs::filter<Components::Camera2D>>();
					if (result.entities.size() == 0)
						return;

					auto& camera = ecs.getComponent<Components::Camera2D>(*result.entities.begin());
					pv = camera.camera.getMatrices().view_projection;
				}

				auto result = ecs.filterEntities<ecs::filter<components::score, Components::Scale2D,
					Components::Position2D>>();

				for (auto entity : result.entities)
				{
					auto& score = ecs.getComponent<components::score>(entity);
					auto& scale2d = ecs.getComponent<Components::Scale2D>(entity);
					auto& pos = ecs.getComponent<Components::Position2D>(entity);

					score.count += count;

					std::string score_text;
					score_text.resize(3);
					score_text.at(2) = '0' + (score.count % 10);
					score_text.at(1) = '0' + ((score.count / 10) % 10);
					score_text.at(0) = '0' + ((score.count / 100) % 10);

					score.set_content(std::move(score_text));

					auto scale = glm::scale(glm::mat4(), glm::vec3(scale2d.scale, 1.0f));
					auto translate = glm::translate(glm::mat4(), glm::vec3(pos.pos, 0.0f));
					auto model = translate * scale;

					glm::mat4 mvp = pv * model;

					score.shader().uniforms().upload<0>(mvp);
				}
			}
		}
	};
}
