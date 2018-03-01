#pragma once
#include "Modules/ECS/System.h"
#include "Modules/Events/EventManager.h"
#include "../Events/BrickEvents.h"

#include <unordered_set>

namespace systems
{
	class collision_system : public MySystem
	{
		events::publisher<events::collision> publisher;

	public:
		collision_system(events::publisher<events::collision> publisher) : 
			publisher(publisher) {}

		void update(ecs_manager& ecs) override
		{
			auto bricks = ecs.filterEntities<ecs::filter<components::brick>>();
			auto walls = ecs.filterEntities<ecs::filter<components::wall>>();
			auto powerups = ecs.filterEntities<ecs::filter<components::powerup>>();
			auto balls = ecs.filterEntities<ecs::filter<components::ball>>();
			auto paddles = ecs.filterEntities<ecs::filter<components::paddle>>();

			for (auto ball : balls.entities) check_collisions_for(ecs, ball, walls.entities);
			for (auto ball : balls.entities) check_collisions_for(ecs, ball, bricks.entities);
			for (auto paddle : paddles.entities) check_collisions_for(ecs, paddle, balls.entities);
			for (auto paddle : paddles.entities) check_collisions_for(ecs, paddle, powerups.entities);
		}

	private:
		bool contains(const std::array<glm::vec2, 4>& cube, const glm::vec2& p)
		{
			return cube[0].x < p.x && p.x < cube[2].x
				&& cube[0].y < p.y && p.y < cube[1].y;
		}

		bool intersects(const std::array<glm::vec2, 4>& c1, const std::array<glm::vec2, 4>& c2) {
			return contains(c1, c2[0])
				|| contains(c1, c2[1])
				|| contains(c1, c2[2])
				|| contains(c1, c2[3])
				|| contains(c2, c1[0])
				|| contains(c2, c1[1])
				|| contains(c2, c1[2])
				|| contains(c2, c1[3]);
		}

		void check_collisions_for(ecs_manager& ecs, uint32_t entity, const std::unordered_set<uint32_t>& candidates)
		{
			auto& entity_pos = ecs.getComponent<Components::Position2D>(entity);
			auto& entity_scale = ecs.getComponent<Components::Scale2D>(entity);

			std::array<glm::vec2, 4> cube = {
				glm::vec2{ -.5f, -.5f },
				{ -.5f, .5f },
				{ .5f, -.5f },
				{ .5f, .5f },
			};

			auto scale = [](std::array<glm::vec2, 4>& cube, const glm::vec2& offset) {
				for (auto& component : cube)
					component *= offset;
			};
			auto move = [](std::array<glm::vec2, 4>& cube, const glm::vec2& offset) {
				for (auto& component : cube)
					component += offset;
			};

			auto entity_cube = cube;

			scale(entity_cube, entity_scale.scale);
			move(entity_cube, entity_pos.pos);

			for (auto it = candidates.begin(); it != candidates.end(); it++)
			{
				auto other_cube = cube;
				auto& other_pos = ecs.getComponent<Components::Position2D>(*it);
				auto& other_scale = ecs.getComponent<Components::Scale2D>(*it);

				scale(other_cube, other_scale.scale);
				move(other_cube, other_pos.pos);

				if (intersects(entity_cube, other_cube))
				{
					publisher.broadcast(events::collision(entity, *it));
				}
			}
		}
	};
}