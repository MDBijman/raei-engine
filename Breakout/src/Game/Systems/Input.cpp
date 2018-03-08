#include "stdafx.h"
#include "Game/Systems/Input.h"

#include "Modules/IO/Input.h"

void Systems::Input::update(ecs_manager& ecs)
{
	auto result = ecs.filterEntities<ecs::filter<Components::Input, Components::Position2D>>();

	for (auto& entity : result.entities)
	{
		auto& input = ecs.getComponent<Components::Input>(entity);
		auto& pos = ecs.getComponent<Components::Position2D>(entity);

		// Lower precision
		float lpDt = static_cast<float>(timer.dt());

		// Move left or right
		if (IO::Keyboard::getKeyStatus('A') == IO::Keyboard::KeyStatus::DOWN)
			pos.pos.x -= input.speed * lpDt;
		if (IO::Keyboard::getKeyStatus('D') == IO::Keyboard::KeyStatus::DOWN)
			pos.pos.x += input.speed * lpDt;
	}
	timer.zero();
}