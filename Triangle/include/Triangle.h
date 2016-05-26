#pragma once
#include "Camera.h"
#include "DrawCall.h"
#include "GraphicsCore.h"

class Triangle
{
public:
	Triangle(HINSTANCE hInstance, HWND window) : camera(glm::vec2(1280, 720), -2.5f, 60.0f, 1.0f, 256.0f)
	{
		graphics = std::make_unique<GraphicsCore>(hInstance, window, "triangle", 1280, 720);
	}

	void begin()
	{
		drawable.push_back(Drawable(*graphics->device, *graphics->physicalDevice, graphics->renderPass, graphics->pipelineCache, graphics->frameBuffers, *graphics->swapchain, graphics->cmdPool, *graphics->queue, camera));
	}

	void draw()
	{
		for(auto it = drawable.begin(); it != drawable.end(); ++it)
			(*it).updateUniformBuffers(camera, *graphics->device, rotation);

		graphics->render(drawable);
	}

	void end()
	{
	}

	float frameTimer = 1.0f;
	float timerSpeed = 0.25f;
	float timer      = 0.0f;
	bool paused      = false;

	float rotationSpeed = 1.0f;
	glm::vec3 rotation = glm::vec3();
	glm::vec2 mousePos;

private:
	std::unique_ptr<GraphicsCore> graphics;

	std::vector<Drawable> drawable;
	Camera camera;
};
