#include "Modules\Importers\Pipeline.h"

#include "Modules\Graphics\VulkanWrappers\VulkanWrappers.h"
#include "Modules\Graphics\Logic\Pipeline.h"
#include "Modules\Importers\Shader.h"

#include <fstream>
#include <sstream>
#include <assert.h>
#include <memory>
#include <vector>
#include <json\JSON.h>

namespace Importers
{
	namespace Pipeline
	{
		Graphics::Pipeline load(const std::string & location, VulkanPipelineLayout & layout, VulkanPipelineVertexInputStateCreateInfo & vi, VulkanRenderPass & rp, VkPipelineCache& cache, VulkanDevice& device)
		{
			JSON::JSON json;
			std::ifstream jsonFile(location);
			if (!jsonFile.is_open())
				assert(!"Cannot open pipeline json file");

			std::stringstream jsonContent;
			jsonContent << jsonFile.rdbuf();
			json = JSON::Parser::parse(jsonContent.str());

			VulkanGraphicsPipelineCreateInfo pipelineCreateInfo;

			auto inputAssemblyState = parseInputAssemblyState(json["input-assembly"]);
			auto rasterizationState = parseRasterizationState(json["rasterization"]);
			auto colorBlendingState = parseBlendAttachmentState(json["color-blending"]);
			auto viewportState = parseViewportState(json["viewport"]);
			auto dynamicStates = parseDynamicStates(json["dynamics"]);
			auto depthAndStencil = parseDepthAndStencilState(json["depth-and-stencil"]);
			auto multisampling = parseMultisamplingState(json["multisampling"]);
			auto shaderStages = parseShaderStages(json["shader-stages"], device);

			pipelineCreateInfo
				.setLayout(layout)
				.setVertexInputState(vi)
				.setInputAssemblyState(inputAssemblyState)
				.setRasterizationState(rasterizationState)
				.setColorBlendState(colorBlendingState)
				.setViewportState(viewportState)
				.setDynamicState(dynamicStates)
				.setDepthStencilState(depthAndStencil)
				.setMultisampleState(multisampling)
				.setStages(shaderStages)
				.setRenderPass(rp);

			VkPipeline pipeline = device.createGraphicsPipelines(cache, pipelineCreateInfo.vkInfo, 1).at(0);
			Graphics::Pipeline graphicsPipeline(pipeline);

			graphicsPipeline.layout = layout;
			return graphicsPipeline;
		}

		VulkanPipelineInputAssemblyStateCreateInfo parseInputAssemblyState(JSON::JSON& json)
		{
			VulkanPipelineInputAssemblyStateCreateInfo inputAssemblyState;
			{
				std::string topology = json["topology"];
				if (topology == "triangle-list")
					inputAssemblyState.setTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				else
					assert(!"Invalid topology state");
			}
			return inputAssemblyState;
		}

		VulkanPipelineRasterizationStateCreateInfo parseRasterizationState(JSON::JSON& json)
		{
			VulkanPipelineRasterizationStateCreateInfo rasterizationState;
			{
				std::string polygonMode = json["polygon-mode"];
				if (polygonMode == "fill")
					rasterizationState.setPolygonMode(VK_POLYGON_MODE_FILL);
				else
					assert(!"Invalid polygon mode state");
			}
			{
				std::string cullMode = json["cull-mode"];
				if (cullMode == "none")
					rasterizationState.setCullMode(VK_CULL_MODE_NONE);
				else
					assert(!"Invalid cull mode state");
			}
			{
				std::string frontFace = json["front-face"];
				if (frontFace == "ccw")
					rasterizationState.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
				else
					assert(!"Invalid front face state");
			}
			{
				bool depthClamp = json["depth-clamp"];
				rasterizationState.setDepthClampEnable(depthClamp);
			}
			{
				bool rasterizerDiscard = json["rasterizer-discard"];
				rasterizationState.setRasterizerDiscardEnable(rasterizerDiscard);
			}
			{
				bool depthBias = json["depth-bias"];
				rasterizationState.setDepthBiasEnable(depthBias);
			}
			return rasterizationState;
		}

		VulkanPipelineColorBlendStateCreateInfo parseBlendAttachmentState(JSON::JSON& json)
		{
			VulkanPipelineColorBlendStateCreateInfo info;
			std::vector<VulkanPipelineColorBlendAttachmentState> colorBlending(1);
			{
				int writeMask = json["color-write-mask"];
				bool blendEnable = json["blending"];
				colorBlending[0] = VulkanPipelineColorBlendAttachmentState()
					.setColorWriteMask(writeMask)
					.setBlendEnable(blendEnable);
			}
			info.setAttachments(colorBlending);
			return info;
		}

		VulkanPipelineViewportStateCreateInfo parseViewportState(JSON::JSON& json)
		{
			VulkanPipelineViewportStateCreateInfo viewport;
			{
				int viewportCount = json["viewport-count"];
				int scissorCount = json["scissor-count"];
				viewport.setViewportCount(viewportCount).setScissorCount(scissorCount);
			}
			return viewport;
		}

		VulkanPipelineDynamicStateCreateInfo parseDynamicStates(JSON::JSON& json)
		{
			std::vector<JSON::JSON> vectorJSON = json;
			VulkanPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
			std::vector<VulkanDynamicState> states;
			for (const std::string& state : vectorJSON)
			{
				if (state == "viewport")
					states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
				if (state == "scissor")
					states.push_back(VK_DYNAMIC_STATE_SCISSOR);
			}
			dynamicStateCreateInfo.setDynamicStates(states);
			return dynamicStateCreateInfo;
		}

		VulkanPipelineDepthStencilStateCreateInfo parseDepthAndStencilState(JSON::JSON & json)
		{
			VulkanPipelineDepthStencilStateCreateInfo state;
			{
				bool depthTest = json["depth-test"];
				state.setDepthTestEnable(depthTest);
			}
			{

				bool depthWrite = json["depth-write"];
				state.setDepthWriteEnable(depthWrite);
			}
			{
				const std::string& depthCompareOp = json["depth-compare-op"];
				if (depthCompareOp == "less-or-equal")
					state.setDepthCompareOp(VK_COMPARE_OP_LESS_OR_EQUAL);
			}
			{
				bool depthBounds = json["depth-bound-test"];
				state.setDepthBoundsTestEnable(depthBounds);
			}
			{
				const std::string& failOp = json["fail-op"];
				if (failOp == "keep")
					state.setFailOp(VK_STENCIL_OP_KEEP);
			}
			{
				const std::string& passOp = json["pass-op"];
				if (passOp == "keep")
					state.setPassOp(VK_STENCIL_OP_KEEP);
			}
			{
				const std::string& compareOp = json["compare-op"];
				if (compareOp == "always")
					state.setCompareOp(VK_COMPARE_OP_ALWAYS);
			}
			{
				bool stencilTest = json["stencil-test"];
				state.setStencilTestEnable(stencilTest);
			}
			{
				const std::string& front = json["front"];
				if (front == "back")
					state.setFront(state.vk.back);
			}
			return state;
		}

		VulkanPipelineMultisampleStateCreateInfo parseMultisamplingState(JSON::JSON & json)
		{
			VulkanPipelineMultisampleStateCreateInfo state;
			{
				const std::string& sampleMask = json["sample-mask"];
				if (sampleMask == "null")
					state.setSampleMask(NULL);
			}
			{
				int rasterizationSamples = json["rasterization-samples"];
				if (rasterizationSamples == 1)
					state.setRasterizationSamples(VK_SAMPLE_COUNT_1_BIT);
			}
			return state;
		}

		std::vector<VulkanPipelineShaderStageCreateInfo> parseShaderStages(JSON::JSON & json, VulkanDevice& device)
		{
			std::vector<VulkanPipelineShaderStageCreateInfo> shaderStages(2);
			const std::string& vertexLocation = json["vertex"];
			const std::string& fragmentLocation = json["fragment"];

			// TODO fix this by loading metadata and retreiving proper info
			shaderStages[0]
				.setStage(VK_SHADER_STAGE_VERTEX_BIT)
				.setModule(Shader::load(vertexLocation.c_str(), device.vkDevice))
				.setName(std::string("main"));

			// TODO fix this by loading metadata and retreiving proper info
			shaderStages[1]
				.setStage(VK_SHADER_STAGE_FRAGMENT_BIT)
				.setModule(Shader::load(fragmentLocation.c_str(), device.vkDevice))
				.setName(std::string("main"));

			return shaderStages;
		}
	};
}
