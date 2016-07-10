#pragma once
#include "Graphics\Renderer\Camera.h"
#include "Graphics\Renderer\Drawable.h"
#include "Graphics\Renderer\Renderer.h"

class Game 
{
public:
	Game(HINSTANCE hInstance, HWND window) : camera(glm::vec2(1280, 720), 60.0f, .1f, 256.0f)
	{
		Graphics::WindowsContext context = {
			hInstance, window, "triangle", 1280, 720
		};
		renderer = new Graphics::Renderer(context);
	}

	void begin()
	{
		auto& device = *renderer->device;
		auto& physicalDevice = *renderer->physicalDevice;
		auto& queue = *renderer->queue;
		auto& swapchain = *renderer->swapchain;

		drawable.push_back(Graphics::Drawable(camera, device, physicalDevice, renderer->cmdPool, queue, renderer->renderPass, renderer->pipelineCache, renderer->frameBuffers, swapchain));
	}

	void draw()
	{
		if (paused) return;

		for (auto it = drawable.begin(); it != drawable.end(); ++it)
			(*it).updateUniformBuffers(camera, *renderer->device);

		renderer->render(drawable);
	}

	void end()
	{
		delete renderer;
	}

	bool paused = false;

	Camera camera;
	float rotationSpeed = 0.3f;
	double dt = 0.0;

private:
	Graphics::Renderer* renderer;
	std::vector<Graphics::Drawable> drawable;
};
