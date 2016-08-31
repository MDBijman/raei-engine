#pragma once
#include "ECSManager.h"

template<class CT, class FT>
class System
{
public:
	virtual void update(ECSManager<CT, FT>& ecs, double dt) = 0;
};
