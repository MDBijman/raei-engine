#pragma once
#include "Camera.h"
#include "Drawable.h"
#include "Renderer.h"

class Triangle
{
public:
	Triangle(HINSTANCE hInstance, HWND window) : camera(glm::vec2(1280, 720), -2.5f, 60.0f, .1f, 256.0f)
	{
		Graphics::WindowsContext context = {
			hInstance, window, "triangle", 1280, 720 
		};
		graphics = std::make_unique<Graphics::Renderer>(context);
	}

	void begin()
	{
		drawable.push_back(Graphics::Drawable(graphics->state, camera));
	}

	void draw()
	{
		for (auto it = drawable.begin(); it != drawable.end(); ++it)
			(*it).updateUniformBuffers(camera);

		graphics->render(drawable);
	}

	void end()
	{
	}

	float frameTimer = 1.0f;
	float timerSpeed = 0.25f;
	float timer      = 0.0f;
	bool paused      = false;

	Camera camera;
	float rotationSpeed = 0.3f;

private:
	std::unique_ptr<Graphics::Renderer> graphics;

	std::vector<Graphics::Drawable> drawable;
};
