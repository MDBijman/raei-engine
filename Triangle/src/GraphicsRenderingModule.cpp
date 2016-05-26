#include "GraphicsRenderingModule.h"

#include "VulkanWrappers.h"
#include "Vertex.h"

GraphicsRenderingModule::GraphicsRenderingModule(VulkanDevice& device, VkCommandPool& cmdPool, VulkanSwapChain& swapchain, VulkanQueue& queue) : device(device), swapchain(swapchain), queue(queue)
{
	VulkanCommandBufferAllocateInfo info;
	info.setCommandPool(cmdPool)
		.setCommandBufferLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
		.setCommandBufferCount(1);

	postPresentCmdBuffer = device.allocateCommandBuffers(info.vkInfo).at(0);

	VulkanSemaphoreCreateInfo semaphoreCreateInfo;

	// This semaphore ensures that the image is complete
	// before starting to submit again
	presentComplete = device.createSemaphore(semaphoreCreateInfo.vkInfo);

	// This semaphore ensures that all commands submitted
	// have been finished before submitting the image to the queue
	renderComplete = device.createSemaphore(semaphoreCreateInfo.vkInfo);
}

void GraphicsRenderingModule::prepare()
{
	device.waitIdle();
	swapchain.acquireNextImage(presentComplete, &currentBuffer);
}

void GraphicsRenderingModule::submit(Drawable& c)
{
	// Get next image in the swap chain (back/front buffer)

	// The submit info structure contains a list of
	// command buffers and semaphores to be submitted to a queue
	// If you want to submit multiple command buffers, pass an array
	VkPipelineStageFlags pipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VulkanSubmitInfo submitInfo;
	std::vector<VkSemaphore> waitSemaphores{ presentComplete };
	std::vector<VkSemaphore> signalSemaphores{ renderComplete };
	std::vector<VkCommandBuffer> commandBuffers{ c.getCommandBuffers()[currentBuffer].vkBuffer };
	submitInfo
		.setDstStageMask(&pipelineStages)
		.setWaitSemaphores(waitSemaphores)
		.setCommandBuffers(commandBuffers)
		.setSignalSemaphores(signalSemaphores);

	// Submit to the graphics queue
	queue.submit(1, submitInfo.vkInfo);
}

void GraphicsRenderingModule::present()
{
	// Present the current buffer to the swap chain
	// This will display the image
	swapchain.queuePresent(queue, currentBuffer);

	// Add a post present image memory barrier
	// This will transform the frame buffer color attachment back
	// to it's initial layout after it has been presented to the
	// windowing system
	// See buildCommandBuffers for the pre present barrier that 
	// does the opposite transformation 
	// Use dedicated command buffer from example base class for submitting the post present barrier

	VulkanImageMemoryBarrier postPresentBarrier;
	postPresentBarrier
		.setSrcAccessMask(0)
		.setDstAccessMask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
		.setOldLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		.setNewLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setSubresourceRange({ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 })
		.setImage(swapchain.buffers[currentBuffer].image);

	VulkanCommandBufferBeginInfo cmdBufInfo;
	postPresentCmdBuffer
		.beginCommandBuffer(cmdBufInfo.vkInfo)
		.putPipelineBarrier(postPresentBarrier.vkBarrier, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
		.end();

	// Initialize submitInfo as an empty object, except for command buffers
	std::vector<VkCommandBuffer> buffers{ postPresentCmdBuffer.vkBuffer };
	VulkanSubmitInfo submitInfo = VulkanSubmitInfo().setCommandBuffers(buffers);

	queue.submit(1, submitInfo.vkInfo);
	queue.waitIdle();
	device.waitIdle();
}