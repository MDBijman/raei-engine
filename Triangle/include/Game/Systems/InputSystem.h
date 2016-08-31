#pragma once
#include "Modules\ECS\ECS.h"
#include "Modules\IO\Input.h"
#include "Modules\IO\Logging.h"
#include "Game\GameConfig.h"
#include <iostream>

namespace Systems
{
	class InputSystem : public MySystem
	{
	public:
		virtual void update(MyECSManager& ecs, double dt)
		{
			auto& entities = ecs.filterEntities<Filter<Components::Input, Components::Velocity3D, Components::Orientation3D>>();
			for (auto& entity : entities)
			{
				auto& input = ecs.getComponent<Components::Input>(entity);
				auto& vel = ecs.getComponent<Components::Velocity3D>(entity);
				auto& orientation = ecs.getComponent<Components::Orientation3D>(entity);

				// Lower precision
				float lpDt = static_cast<float>(dt);

				if (input.keyboardMode == Components::Input::KeyboardMode::WASD)
				{
					// Move forward or backward
					if (Input::Keyboard::getKeyStatus('W') == Input::Keyboard::KeyStatus::DOWN)
						vel.vel += orientation.front * lpDt * 3.0f;

					if (Input::Keyboard::getKeyStatus('S') == Input::Keyboard::KeyStatus::DOWN)
						vel.vel -= orientation.front * lpDt * 3.0f;

					// Move left or right
					if (Input::Keyboard::getKeyStatus('A') == Input::Keyboard::KeyStatus::DOWN)
						vel.vel -= glm::cross(orientation.front, orientation.up) * lpDt * 3.0f;

					if (Input::Keyboard::getKeyStatus('D') == Input::Keyboard::KeyStatus::DOWN)
						vel.vel += glm::cross(orientation.front, orientation.up) * lpDt * 3.0f;
				}

				if (input.mouseMode == Components::Input::MouseMode::FIRST_PERSON)
				{
					orientation.rotate(Input::Mouse::getDX() * 20.0f, -(Input::Mouse::getDY() * 20.0f), lpDt);
				}
			}
		}
	};
}
