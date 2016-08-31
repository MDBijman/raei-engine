#pragma once
#include "Modules\ECS\ECS.h"
#include "Modules\IO\Input.h"
#include "Modules\IO\Logging.h"
#include "Game\GameConfig.h"

namespace Systems
{
	class Input : public MySystem
	{
	public:
		virtual void update(MyECSManager& ecs, double dt)
		{
			auto& entities = ecs.filterEntities<Filter<Components::Input, Components::Position2D, Components::Velocity2D>>();
			for (auto& entity : entities)
			{
				auto& input = ecs.getComponent<Components::Input>(entity);
				auto& pos = ecs.getComponent<Components::Position2D>(entity);
				auto& vel = ecs.getComponent<Components::Velocity2D>(entity);

				// Lower precision
				float lpDt = static_cast<float>(dt);

				if (input.keyboardMode == Components::Input::KeyboardMode::WASD)
				{
					// Move left or right
					if (IO::Keyboard::getKeyStatus('A') == IO::Keyboard::KeyStatus::DOWN)
						pos.pos -= vel.vel * lpDt;
					if (IO::Keyboard::getKeyStatus('D') == IO::Keyboard::KeyStatus::DOWN)
						pos.pos += vel.vel * lpDt;
				}
			}
		}
	};
}
