#pragma once
#include "Modules/Graphics/Logic/Renderer.h"
#include "Modules/Graphics/Data/Shaders/Shader.h"
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

	template<class ShaderType>
	void render(Components::CommandBuffers& buffers, ShaderType& shader, Components::Camera2D& camera) const
	{
		shader.updateUniformBuffers(camera, renderer->context->device);
		renderer->submit(buffers.commandBuffers->at(renderer->getCurrentBuffer()));
	}

	void present() const
	{
		renderer->present();
	}

	Graphics::Renderer* renderer;
};
