#pragma once
#include "Modules/Graphics/Logic/Renderer.h"
#include "Components/Components.h"

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
	}

	void prepare() const
	{
		renderer->prepare();
	}

	void render(Components::CommandBuffers& buffers, Components::MeshShader& shader, Components::Camera2D& camera) const
	{
		shader.updateUniformBuffers(camera, *renderer->device);
		renderer->submit(buffers.commandBuffers[renderer->getCurrentBuffer()].vkBuffer);
	}

	void present() const
	{
		renderer->present();
	}

	Graphics::Renderer* renderer;
};
