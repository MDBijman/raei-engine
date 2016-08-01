#pragma once
#include "ECSManager.h"

class System
{
public:
	template<class CT, class FT>
	virtual void update(ECSManager<CT, FT> ecs, float dt) = 0;
};
