#pragma once
#include <assert.h>
#include <vulkan\vulkan.h>

class VulkanCommandBuffer
{
public:
	// Initializes the vk buffer as VK_NULL_HANDLE
	VulkanCommandBuffer() : vkBuffer(VK_NULL_HANDLE) {}
	VulkanCommandBuffer(VkCommandBuffer b) : vkBuffer(b) {}

	void beginCommandBuffer(VkCommandBufferBeginInfo& vkInfo)
	{
		VkResult error = vkBeginCommandBuffer(vkBuffer, &vkInfo);
		assert(!error);
	}

	void end()
	{
		VkResult error = vkEndCommandBuffer(vkBuffer);
		assert(!error);
	}

	void beginRenderPass(VkRenderPassBeginInfo& vkInfo, VkSubpassContents contents)
	{
		vkCmdBeginRenderPass(vkBuffer, &vkInfo, contents);
	}

	void endRenderPass()
	{
		vkCmdEndRenderPass(vkBuffer);
	}
	
	void setViewport(VkViewport& port)
	{
		vkCmdSetViewport(vkBuffer, 0, 1, &port);
	}

	void setScissors(VkRect2D& scissor)
	{
		vkCmdSetScissor(vkBuffer, 0, 1, &scissor);
	}

	void bindDescriptorSet(VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet)
	{
		vkCmdBindDescriptorSets(vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
	}

	void bindPipeline(VkPipeline& pipeline)
	{
		vkCmdBindPipeline(vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void bindVertexBuffers(VkBuffer& vertexBuffer)
	{
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(vkBuffer, VERTEX_BUFFER_BIND_ID, 1, &vertexBuffer, offsets);
	}

	void bindIndexBuffers(VkBuffer& indexBuffer)
	{
		vkCmdBindIndexBuffer(vkBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void drawIndexed(int count)
	{
		vkCmdDrawIndexed(vkBuffer, count, 1, 0, 0, 1);
	}

	void putPipelineBarrier(VkImageMemoryBarrier& barrier)
	{
		vkCmdPipelineBarrier(
			vkBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	VkCommandBuffer vkBuffer;
};
