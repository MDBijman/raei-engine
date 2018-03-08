#include "stdafx.h"
#include "Game/Systems/PowerupSystem.h"
#include "Modules/Graphics/Graphics.h"

void systems::powerup_system::update(ecs_manager& ecs)
{
	auto bricks = ecs.filterEntities<ecs::filter<components::brick>>();
	auto balls = ecs.filterEntities<ecs::filter<components::ball>>();
	auto paddles = ecs.filterEntities<ecs::filter<components::paddle>>();

	while (collision_subscriber.size() > 0)
	{
		auto collision = collision_subscriber.pop();
		// Bricks
		if (balls.entities.find(collision->a) != balls.entities.end()
			&& bricks.entities.find(collision->b) != bricks.entities.end())
		{
			on_brick_hit(ecs, collision->b);
		}
		else if (balls.entities.find(collision->b) != balls.entities.end()
			&& bricks.entities.find(collision->a) != bricks.entities.end())
		{
			on_brick_hit(ecs, collision->a);
		}
		// Powerups
		else if (paddles.entities.find(collision->b) != paddles.entities.end()
			&& live_powerups.find(collision->a) != live_powerups.end())
		{
			on_powerup_hit(ecs, collision->a);
		}
		else if (paddles.entities.find(collision->a) != paddles.entities.end()
			&& live_powerups.find(collision->b) != live_powerups.end())
		{
			on_powerup_hit(ecs, collision->b);
		}
	}
}

void systems::powerup_system::on_brick_hit(ecs_manager& ecs, uint32_t brick_id)
{
	auto& brick_position = ecs.getComponent<Components::Position2D>(brick_id);

	int random = std::rand();
	float zero_to_one = static_cast<float>(random) / static_cast<float>(RAND_MAX);

	if (zero_to_one < 0.1f) // Powerup
	{
		auto new_powerup = ecs.createEntity();
		live_powerups.insert(new_powerup);

		ecs.addComponent(new_powerup, Components::Position2D(brick_position));
		ecs.addComponent(new_powerup, Components::Velocity2D(0.0f, 0.2f));
		ecs.addComponent(new_powerup, Components::Scale2D(0.03f, 0.03f));
		ecs.addComponent(new_powerup, components::collider());
		ecs.addComponent(new_powerup, components::powerup());

		auto attributes = graphics.resources.create_attributes<shaders::sprite_attributes>({
			{
				{ -.5f, -.5f },
				{ 0.0f, 0.0f }
			},
			{
				{ -.5f, .5f },
				{ 0.0f, 1.0f }
			},
			{
				{ .5f, -.5f },
				{ 1.0f, 0.0f }
			},
			{
				{ .5f, .5f },
				{ 1.0f, 1.0f }
			}
			});
		auto indices = graphics.resources.create_indices({ 0, 1, 2, 2, 1, 3 });
		auto uniform = graphics.resources.create_uniform<shaders::sprite_uniforms>({
			graphics.resources.create_buffer<glm::mat4>(
				camera.getMatrices().projection * camera.getMatrices().view * glm::mat4(), 0),
			graphics.resources.create_texture("./res/textures/powerup.dds", vk::Format::eBc3UnormBlock, 1,
				vk::ShaderStageFlagBits::eFragment)
			});

		auto& drawable = ecs.addComponent(new_powerup, components::drawable<shaders::sprite_shader>(
			graphics.resources.create_drawable(shaders::sprite_shader(
				std::move(attributes),
				std::move(indices),
				std::move(uniform)))));

		std::cout << "new powerup\n";
	}
}

void systems::powerup_system::on_powerup_hit(ecs_manager& ecs, uint32_t powerup_id)
{
	auto powerup_position = ecs.getComponent<Components::Position2D>(powerup_id);

	auto ball = ecs.createEntity();
	ecs.addComponent(ball, Components::Position2D(powerup_position));
	ecs.addComponent(ball, Components::Velocity2D(0.0, -1.6));
	ecs.addComponent(ball, Components::Scale2D(.03, .03));
	ecs.addComponent(ball, components::collider());
	ecs.addComponent(ball, components::ball());

	auto attributes = graphics.resources.create_attributes<shaders::sprite_attributes>({
		{
			{ -.5f, -.5f },
			{ 0.0f, 0.0f }
		},
		{
			{ -.5f, .5f },
			{ 0.0f, 1.0f }
		},
		{
			{ .5f, -.5f },
			{ 1.0f, 0.0f }
		},
		{
			{ .5f, .5f },
			{ 1.0f, 1.0f }
		}
		});
	auto indices = graphics.resources.create_indices({ 0, 1, 2, 2, 1, 3 });
	auto uniform = graphics.resources.create_uniform<shaders::sprite_uniforms>({
		graphics.resources.create_buffer<glm::mat4>(
			camera.getMatrices().projection * camera.getMatrices().view * glm::mat4(), 0),
		graphics.resources.create_texture("./res/textures/paddle.dds", vk::Format::eBc3UnormBlock, 1,
			vk::ShaderStageFlagBits::eFragment)
		});

	auto& drawable = ecs.addComponent(ball, components::drawable<shaders::sprite_shader>(
		graphics.resources.create_drawable(shaders::sprite_shader(
			std::move(attributes),
			std::move(indices),
			std::move(uniform)))));

	std::cout << "caught powerup\n";
	ecs.removeEntity(powerup_id);
}