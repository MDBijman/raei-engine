#pragma once
#include <assert.h>
#include <vulkan\vulkan.h>

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer(VkCommandBuffer b) : buffer(b) {}

	void beginCommandBuffer(VkCommandBufferBeginInfo& info)
	{
		VkResult error = vkBeginCommandBuffer(buffer, &info);
		assert(!error);
	}

	void end()
	{
		VkResult error = vkEndCommandBuffer(buffer);
		assert(!error);
	}

	void beginRenderPass(VkRenderPassBeginInfo& info, VkSubpassContents contents)
	{
		vkCmdBeginRenderPass(buffer, &info, contents);
	}

	void endRenderPass()
	{
		vkCmdEndRenderPass(buffer);
	}
	
	void setViewport(VkViewport& port)
	{
		vkCmdSetViewport(buffer, 0, 1, &port);
	}

	void setScissors(VkRect2D& scissor)
	{
		vkCmdSetScissor(buffer, 0, 1, &scissor);
	}

	void bindDescriptorSet(VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet)
	{
		vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
	}

	void bindPipeline(VkPipeline& pipeline)
	{
		vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void bindVertexBuffers(VkBuffer& vertexBuffer)
	{
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(buffer, VERTEX_BUFFER_BIND_ID, 1, &vertexBuffer, offsets);
	}

	void bindIndexBuffers(VkBuffer& indexBuffer)
	{
		vkCmdBindIndexBuffer(buffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void drawIndexed(int count)
	{
		vkCmdDrawIndexed(buffer, count, 1, 0, 0, 1);
	}

	void putPipelineBarrier(VkImageMemoryBarrier& barrier)
	{
		vkCmdPipelineBarrier(
			buffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	operator VkCommandBuffer() const { return buffer; }
	VkCommandBuffer& getBuffer()
	{
		return buffer;
	}
private:
	VkCommandBuffer buffer;
};
