#pragma once
#include "ecs\ECS.h"
#include "Game\Components\Position3D.h"
#include <iostream>

class MovementSystem : public System
{
public:
	template<class CT, class FT>
	virtual void update(ECSManager<CT, FT> ecs, float dt)
	{
		

		std::cout << "ECM" << std::endl;
	}
};

