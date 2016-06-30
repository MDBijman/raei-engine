#pragma once
#define VERTEX_BUFFER_BIND_ID 0

#include <vulkan\vulkan.h>
#include <glm\glm.hpp>
#include "VulkanWrappers.h"
#include "GraphicsStateBucket.h"
#include "Texture.h"
#include "Camera.h"
#include "Vertices.h"
#include "Indices.h"
#include "UniformData.h"

class Drawable
{
public:
	Drawable(GraphicsStateBucket& state, Camera& camera);

	/*
	* Initialization Methods
	*/

	void prepareTexture();
	void prepareVertices();
	void prepareUniformBuffers(Camera& camera);
	void prepareDescriptorSetLayout();
	void preparePipeline();
	void prepareDescriptorPool();
	void updateDescriptorSet();
	void prepareCommandBuffers(Camera& camera);

	void updateUniformBuffers(Camera& camera);

	std::vector<VulkanCommandBuffer>& getCommandBuffers();

private:
	const std::string SHADER_LOCATION = "./../data/shaders/";
	const std::string SHADER_NAME = "mesh";
	const std::string VERTEX_LOCATION = SHADER_LOCATION + SHADER_NAME + ".vert.spv";
	const std::string FRAGMENT_LOCATION = SHADER_LOCATION + SHADER_NAME + ".frag.spv";

	Texture texture;
	Vertices vertices;
	Indices indices;
	UniformData uniformDataVS;

	struct {
		glm::mat4 projectionMatrix;
		glm::mat4 modelMatrix;
		glm::mat4 viewMatrix;
	} uboVS;

	struct {
		VkPipeline solid;
	} pipelines;

	VkPipelineLayout      pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet       descriptorSet;
	VkDescriptorPool      descriptorPool;

	GraphicsStateBucket& state;

	std::vector<VulkanCommandBuffer> commandBuffers;
};
