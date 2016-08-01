#pragma once
#include <assert.h>
#include <vulkan\vulkan.h>

#define VERTEX_BUFFER_BIND_ID 0

class VulkanCommandBuffer
{
public:
	// Initializes the vk buffer as VK_NULL_HANDLE
	VulkanCommandBuffer() : vkBuffer(VK_NULL_HANDLE) {}
	VulkanCommandBuffer(VkCommandBuffer b) : vkBuffer(b) {}

	VulkanCommandBuffer& beginCommandBuffer(VkCommandBufferBeginInfo& vkInfo)
	{
		VkResult error = vkBeginCommandBuffer(vkBuffer, &vkInfo);
		assert(!error);
		return *this;
	}

	void end()
	{
		VkResult error = vkEndCommandBuffer(vkBuffer);
		assert(!error);
	}

	VulkanCommandBuffer& beginRenderPass(VkRenderPassBeginInfo& vkInfo, VkSubpassContents contents)
	{
		vkCmdBeginRenderPass(vkBuffer, &vkInfo, contents);
		return *this;
	}

	VulkanCommandBuffer& endRenderPass()
	{
		vkCmdEndRenderPass(vkBuffer);
		return *this;
	}
	
	VulkanCommandBuffer& setViewport(VkViewport& port)
	{
		vkCmdSetViewport(vkBuffer, 0, 1, &port);
		return *this;
	}

	VulkanCommandBuffer& setScissors(VkRect2D& scissor)
	{
		vkCmdSetScissor(vkBuffer, 0, 1, &scissor);
		return *this;
	}

	VulkanCommandBuffer& bindDescriptorSet(VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet)
	{
		vkCmdBindDescriptorSets(vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
		return *this;
	}

	VulkanCommandBuffer& bindPipeline(VkPipeline& pipeline)
	{
		vkCmdBindPipeline(vkBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		return *this;
	}

	VulkanCommandBuffer& bindVertexBuffers(VkBuffer& vertexBuffer)
	{
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(vkBuffer, VERTEX_BUFFER_BIND_ID, 1, &vertexBuffer, offsets);
		return *this;
	}

	VulkanCommandBuffer& bindIndexBuffers(VkBuffer& indexBuffer)
	{
		vkCmdBindIndexBuffer(vkBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		return *this;
	}

	VulkanCommandBuffer& drawIndexed(int count)
	{
		vkCmdDrawIndexed(vkBuffer, count, 1, 0, 0, 1);
		return *this;
	}

	VulkanCommandBuffer& putPipelineBarrier(const VkImageMemoryBarrier& barrier, const VkPipelineStageFlags& pipelineStage)
	{
		vkCmdPipelineBarrier(
			vkBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			pipelineStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
		return *this;
	}

	VkCommandBuffer vkBuffer;
};
