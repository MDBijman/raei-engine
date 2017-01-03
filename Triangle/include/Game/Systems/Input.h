#pragma once
#include "Modules/ECS/ECS.h"
#include "Modules/IO/Input.h"
#include "Game/GameConfig.h"

namespace Systems
{
	class Input : public MySystem
	{
	public:
		void update(MyECSManager& ecs, double dt) override
		{
			auto&& entities = ecs.filterEntities<Filter<Components::Input, Components::Position3D>>();
			for (auto& entity : entities)
			{
				auto& input = ecs.getComponent<Components::Input>(entity);
				auto& pos = ecs.getComponent<Components::Position3D>(entity);

				// Lower precision
				float lpDt = static_cast<float>(dt);

				// Move left or right
				if (IO::Keyboard::getKeyStatus('A') == IO::Keyboard::KeyStatus::DOWN)
					pos.pos.x -= input.speed * lpDt;
				if (IO::Keyboard::getKeyStatus('D') == IO::Keyboard::KeyStatus::DOWN)
					pos.pos.x += input.speed * lpDt;		
			}
		}
	};
}
