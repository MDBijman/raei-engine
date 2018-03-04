#pragma once
#include <glm/detail/type_vec2.hpp>

#include "Swapchain.h"
#include "Frame.h"
#include "VulkanContext.h"
#include "Resources.h"
#include "../Shaders/Uniforms/Texture.h"
#include "../Drawables/DrawableFactory.h"


namespace graphics
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

		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanSwapChain> swapchain;
		speck::graphics::resource_loader resources;
		speck::graphics::drawable_factory factory;

		vk::Format depthFormat;

		std::vector<vk::Framebuffer> frameBuffers;

		uint32_t getCurrentBuffer() const;

		Frame getFrame();
		void submitFrame(Frame* frame);

	private:
		void prepare();
		void submit(vk::CommandBuffer* buffer) const;
		void present();

		vk::RenderPass drawPass;
		struct
		{
			vk::Image        image;
			vk::DeviceMemory mem;
			vk::ImageView    view;
		} depthStencil;

		vk::PipelineStageFlags submitPipelineStages = vk::PipelineStageFlagBits::eBottomOfPipe;

		const glm::vec2 SCREEN_DIMENSIONS;

		void submitPrePresentBarrier(vk::Image image) const;
		void submitPostPresentBarrier(vk::Image image) const;

		void prepareDepthStencil(uint32_t width, uint32_t height);
		void prepareRenderPass();

		void preparePipelineCache();
		void prepareFramebuffers(uint32_t width, uint32_t height);
		uint32_t getMemoryPropertyIndex(vk::MemoryPropertyFlags flag, vk::MemoryRequirements requirements) const;

		uint32_t currentBuffer = 0;

		vk::Semaphore          presentComplete;
		vk::Semaphore          renderComplete;
		vk::Fence waitImage;

		vk::CommandBuffer      postPresentCmdBuffer;
		vk::CommandBuffer      prePresentCmdBuffer;
	};
}
