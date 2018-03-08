#include "stdafx.h"
#include "Game/Systems/Exit.h"

#include "Modules/IO/Input.h"

void Systems::Exit::update(ecs_manager& ecs)
{
	auto status = IO::Keyboard::getKeyStatus(VK_ESCAPE);
	if (status == IO::Keyboard::KeyStatus::DOWN)
	{
		exit(0);
	}
}