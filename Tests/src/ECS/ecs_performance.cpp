#include <catch/catch.hpp>
#include <chrono>
#include "Game/ECSConfig.h"


SCENARIO("ECS performance") {
	GIVEN("ecs") {
		ecs_manager ecs;
		auto e = ecs.createEntity();
		ecs.addComponent(e, Components::Position2D(glm::vec2()));

		auto zero = std::chrono::high_resolution_clock::now();

		int i = 0;
		for (; i < 1000000000; i++)
		{
			auto component = ecs.getComponent<Components::Position2D>(e);
		}
		std::cout << i << std::endl;

		auto diff = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - zero);
		std::cout << diff.count() << std::endl;
	}
}