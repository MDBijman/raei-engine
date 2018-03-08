#include "stdafx.h"
#include "Game/Systems/Movement2D.h"

#include "Game/Components/Position2D.h"
#include "Game/Components/Velocity2D.h"

void Systems::Movement2D::update(ecs_manager& ecs)
{
	using namespace Components;
	auto result = ecs.filterEntities<ecs::filter<Position2D, Velocity2D>>();
	for (auto& entity : result.entities)
	{
		auto& pos = ecs.getComponent<Position2D>(entity);
		auto& vel = ecs.getComponent<Velocity2D>(entity);

		pos.pos += vel.vel * static_cast<float>(timer.dt());
	}
	timer.zero();
}
