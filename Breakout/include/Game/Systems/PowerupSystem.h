#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"
#include "Game/Events/BrickEvents.h"

namespace graphics
{
	class Renderer;
	class Camera;
}

namespace systems
{
	class powerup_system : public MySystem
	{
		events::subscriber<events::collision>& collision_subscriber;
		std::unordered_set<uint32_t> live_powerups;
		graphics::Renderer& graphics;
		graphics::Camera& camera;

	public:
		powerup_system(events::subscriber<events::collision>& sub, graphics::Renderer& r, graphics::Camera& c) :
			collision_subscriber(sub), graphics(r), camera(c)
		{}

		void update(ecs_manager& ecs) override;

	private:
		void on_brick_hit(ecs_manager& ecs, uint32_t brick_id);

		void on_powerup_hit(ecs_manager& ecs, uint32_t powerup_id);
	};
}