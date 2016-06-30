#pragma once
#include <math.h>
#include <vector>
#include <chrono>
#include <memory>
#include <glm\glm.hpp>

#include "VulkanWrappers.h"
#include "Shaders.h"
#include "StateBucket.h"
#include "Drawable.h"

namespace Graphics
{
	struct WindowsContext 
	{
		HINSTANCE hInstance;
		HWND window;
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	class Renderer
	{
	public:
		Renderer(WindowsContext context);

		void prepare();
		void submit(Drawable& c);
		void present();
		void render(std::vector<Drawable> d);

		StateBucket state;
		
	private:
		const glm::vec2 SCREEN_DIMENSIONS;

		void submitPrePresentBarrier(VkImage image);
		void submitPostPresentBarrier(VkImage image);

		void prepareDepthStencil(uint32_t width, uint32_t height);
		void prepareRenderPass();
		void preparePipelineCache();
		void prepareFramebuffers(uint32_t width, uint32_t height);

		uint32_t currentBuffer = 0;

		VkSemaphore          presentComplete;
		VkSemaphore          renderComplete;

		VulkanCommandBuffer  postPresentCmdBuffer;
		VulkanCommandBuffer  prePresentCmdBuffer;
	};
}
