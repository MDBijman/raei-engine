#pragma once
#include "Game/ECSConfig.h"
#include "Game/EventConfig.h"

namespace systems
{
	class collision_system : public MySystem
	{
		events::publisher<events::collision>& publisher;

	public:
		collision_system(events::publisher<events::collision>& publisher) : 
			publisher(publisher) {}

		void update(ecs_manager& ecs) override;

	private:
	
		bool contains(const std::array<glm::vec2, 4>& cube, const glm::vec2& p);
		bool intersects(const std::array<glm::vec2, 4>& c1, const std::array<glm::vec2, 4>& c2);
		void check_collisions_for(ecs_manager& ecs, uint32_t entity, const std::unordered_set<uint32_t>& candidates);
	};
}