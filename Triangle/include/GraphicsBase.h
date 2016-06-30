#pragma once
#include <glm\glm.hpp>
#include <memory>
#include <vector>

#include "VulkanWrappers.h"
#include "Renderer.h"
#include "GraphicsStateBucket.h"
class Drawable;

class GraphicsBase 
{
public:
	GraphicsStateBucket state;

	struct
	{
		std::unique_ptr<Renderer> renderer;
	} modules;

	const glm::vec2 SCREEN_DIMENSIONS;

	/*
	* Initializes a unique Vulkan Instance for this application.
	* Also initializes physical devices, queue family properties, physical device memory properties, the swapchain, and the queue.
	*/
	GraphicsBase(HINSTANCE hInstance, HWND window, std::string name, uint32_t width, uint32_t height);

	void render(std::vector<Drawable> d);

private:
	void prepareDepthStencil(uint32_t width, uint32_t height);
	void prepareRenderPass();
	void preparePipelineCache();
	void prepareFramebuffers(uint32_t width, uint32_t height);

	
};