#pragma once
#include <glm\glm.hpp>
#include "Drawable.h"
#include "Camera.h"

struct DrawCall
{
	Drawable& drawable;
	Camera& camera;
};
