pos = createComponent(ecs, {
	name = "Position2D",
	args = { 0.0, 0.8 }
})
vel = createComponent(ecs, {
	name = "Velocity2D",
	args = { 0.0, 0.0 }
})
scale = createComponent(ecs, {
	name = "Scale2D",
	args = { 0.2, 0.05 }
})
input = createComponent(ecs, {
	name = "Input",
	args = { 1.0 }
})
spriteData = createComponent(ecs, {
	name = "SpriteAttributes",
	args = {
		{
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
		},
		{ 0, 1, 2, 2, 1, 3 }
	}
})
uniform = createComponent(ecs, {
	name = "SpriteUniforms",
	args = {
		graphicsContext(),
		-- More
	}
})
shader = createComponent(ecs, {
	name = "SpriteShader",

})

scale = createComponent(ecs, entity[1])

createEntity(ecs, { pos, scale })

create(ecs, entity)