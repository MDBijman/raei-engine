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
	prePresentCmdBuffer = device.allocateCommandBuffers(info.vkInfo).at(0);

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
	swapchain.acquireNextImage(presentComplete, &currentBuffer);
	submitPostPresentBarrier(swapchain.buffers[currentBuffer].image);
}

void GraphicsRenderingModule::submit(Drawable& c)
{
	// The submit info structure contains a list of
	// command buffers and semaphores to be submitted to a queue
	// If you want to submit multiple command buffers, pass an array
	VulkanSubmitInfo submitInfo;
	VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	std::vector<VkCommandBuffer> commandBuffers{ c.getCommandBuffers()[currentBuffer].vkBuffer };
	std::vector<VkSemaphore> wait{presentComplete};
	std::vector<VkSemaphore> signal{renderComplete};
	submitInfo.setCommandBuffers(commandBuffers)
		.setDstStageMask(&submitPipelineStages)
		.setSignalSemaphores(signal)
		.setWaitSemaphores(wait);

	// Submit to the graphics queue
	queue.submit(1, submitInfo.vkInfo);
}

void GraphicsRenderingModule::present()
{
	submitPrePresentBarrier(swapchain.buffers[currentBuffer].image);

	swapchain.queuePresent(queue, currentBuffer, renderComplete);

	VkResult err = (vkQueueWaitIdle(queue));
	assert(!err);
}

void GraphicsRenderingModule::submitPostPresentBarrier(VkImage image)
{
	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufInfo.pNext = NULL;

	VkResult err = (vkBeginCommandBuffer(postPresentCmdBuffer.vkBuffer, &cmdBufInfo));
	assert(!err);

	VkImageMemoryBarrier postPresentBarrier = {};
	postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	postPresentBarrier.pNext = NULL;
	postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	postPresentBarrier.srcAccessMask = 0;
	postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	postPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	postPresentBarrier.image = image;

	vkCmdPipelineBarrier(
		postPresentCmdBuffer.vkBuffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, nullptr, // No memory barriers,
		0, nullptr, // No buffer barriers,
		1, &postPresentBarrier);

	err = (vkEndCommandBuffer(postPresentCmdBuffer.vkBuffer));
	assert(!err);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &postPresentCmdBuffer.vkBuffer;

	err = (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
	assert(!err);
}

void GraphicsRenderingModule::submitPrePresentBarrier(VkImage image)
{
	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBufInfo.pNext = NULL;

	VkResult err = (vkBeginCommandBuffer(prePresentCmdBuffer.vkBuffer, &cmdBufInfo));
	assert(!err);

	VkImageMemoryBarrier prePresentBarrier = {};
	prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	prePresentBarrier.pNext = NULL;
	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	prePresentBarrier.dstAccessMask = 0;
	prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	prePresentBarrier.image = image;

	vkCmdPipelineBarrier(
		prePresentCmdBuffer.vkBuffer,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, nullptr, // No memory barriers,
		0, nullptr, // No buffer barriers,
		1, &prePresentBarrier);

	err = (vkEndCommandBuffer(prePresentCmdBuffer.vkBuffer));
	assert(!err);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &prePresentCmdBuffer.vkBuffer;

	err = (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
	assert(!err);
}