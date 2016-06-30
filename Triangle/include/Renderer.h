#pragma once
#include <math.h>
#include <vector>
#include <chrono>
#include <memory>

#include "VulkanWrappers.h"
#include "Shaders.h"
#include "GraphicsStateBucket.h"
#include "Drawable.h"

class Renderer
{
public:
	Renderer(GraphicsStateBucket& state);
	
	void prepare();
	void submit(Drawable& c);
	void present();

private:
	GraphicsStateBucket& state;

	void submitPrePresentBarrier(VkImage image);
	void submitPostPresentBarrier(VkImage image);

	uint32_t currentBuffer = 0;

	VkSemaphore          presentComplete;
	VkSemaphore          renderComplete;

	VulkanCommandBuffer  postPresentCmdBuffer;
	VulkanCommandBuffer  prePresentCmdBuffer;
};