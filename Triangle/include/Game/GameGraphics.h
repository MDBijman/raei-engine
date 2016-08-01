#pragma once
#include "Modules\Graphics\Logic\Renderer.h"
#include "Modules\Graphics\Logic\Drawable.h"
#include "Modules\Graphics\Logic\Camera.h"

#include <vector>

class GameGraphics
{
public:
	GameGraphics(HINSTANCE hInstance, HWND window)
	{
		Graphics::WindowsContext context = {
				hInstance, window, "triangle", 1280, 720
		};
		renderer = new Graphics::Renderer(context);
		camera = Camera(glm::vec2(1280, 720), 60.0f, .1f, 256.0f);

		auto& device = *renderer->device;
		auto& physicalDevice = *renderer->physicalDevice;
		auto& queue = *renderer->queue;
		auto& swapchain = *renderer->swapchain;

		drawable.push_back(Graphics::Drawable(camera, device, physicalDevice, renderer->cmdPool, queue, renderer->renderPass, renderer->pipelineCache, renderer->frameBuffers, swapchain));
	}

	void render()
	{
		for (auto it = drawable.begin(); it != drawable.end(); ++it)
			(*it).updateUniformBuffers(camera, *renderer->device);
		renderer->render(drawable);
	}

	Camera camera;
private:
	Graphics::Renderer* renderer;
	std::vector<Graphics::Drawable> drawable;
};
