#include "Modules/Importers/PipelineImporter.h"

#include "Modules/Graphics/Logic/Pipeline.h"
#include "Modules/Importers/Shader.h"
#include "Modules/Importers/JSON.h"

#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>

namespace Importers
{
	namespace Pipeline
	{
		Graphics::Pipeline load(const std::string & location, const vk::PipelineLayout & layout, const vk::PipelineVertexInputStateCreateInfo & vi, const vk::RenderPass & rp, const vk::PipelineCache & cache, const vk::Device & device)
		{
			JSON::JSON json;
			std::ifstream jsonFile(location);
			if(!jsonFile.is_open())
				assert(!"Cannot open pipeline json file");

			std::stringstream jsonContent;
			jsonContent << jsonFile.rdbuf();
			json = JSON::Parser::parse(jsonContent.str());

			Graphics::Pipeline graphicsPipeline;
			graphicsPipeline.layout = layout;
			graphicsPipeline.inputAssemblyInfo = parseInputAssemblyState(json["input-assembly"]);
			graphicsPipeline.rasterizationInfo = parseRasterizationState(json["rasterization"]);
			graphicsPipeline.colorBlendInfo = parseBlendAttachmentState(json["color-blending"], graphicsPipeline);
			graphicsPipeline.viewportInfo = parseViewportState(json["viewport"]);
			graphicsPipeline.dynamicStateInfo = parseDynamicStates(json["dynamics"], graphicsPipeline);
			graphicsPipeline.depthStencilInfo = parseDepthAndStencilState(json["depth-and-stencil"]);
			graphicsPipeline.multisampleInfo = parseMultisamplingState(json["multisampling"]);
			graphicsPipeline.shaderStagesInfo = parseShaderStages(json["shader-stages"], device);

			vk::GraphicsPipelineCreateInfo pipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
				.setLayout(layout)
				.setPVertexInputState(&vi)
				.setPInputAssemblyState(&graphicsPipeline.inputAssemblyInfo)
				.setPRasterizationState(&graphicsPipeline.rasterizationInfo)
				.setPColorBlendState(&graphicsPipeline.colorBlendInfo)
				.setPViewportState(&graphicsPipeline.viewportInfo)
				.setPDynamicState(&graphicsPipeline.dynamicStateInfo)
				.setPDepthStencilState(&graphicsPipeline.depthStencilInfo)
				.setPMultisampleState(&graphicsPipeline.multisampleInfo)
				.setStageCount(graphicsPipeline.shaderStagesInfo.size())
				.setPStages(graphicsPipeline.shaderStagesInfo.data())
				.setRenderPass(rp);
			graphicsPipeline.vk = device.createGraphicsPipelines(cache, pipelineCreateInfo).at(0);

			return graphicsPipeline;
		}

		vk::PipelineInputAssemblyStateCreateInfo parseInputAssemblyState(JSON::JSON& json)
		{
			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
			{
				std::string topology = json["topology"];
				if(topology == "triangle-list")
					inputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList);
				else
					assert(!"Invalid topology state");
			}
			return inputAssemblyState;
		}

		vk::PipelineRasterizationStateCreateInfo parseRasterizationState(JSON::JSON& json)
		{
			vk::PipelineRasterizationStateCreateInfo rasterizationState;
			{
				std::string polygonMode = json["polygon-mode"];
				if(polygonMode == "fill")
					rasterizationState.setPolygonMode(vk::PolygonMode::eFill);
				else
					assert(!"Invalid polygon mode state");
			}
			{
				std::string cullMode = json["cull-mode"];
				if(cullMode == "none")
					rasterizationState.setCullMode(vk::CullModeFlagBits::eNone);
				else if(cullMode == "back")
					rasterizationState.setCullMode(vk::CullModeFlagBits::eBack);
				else
					assert(!"Invalid cull mode state");
			}
			{
				std::string frontFace = json["front-face"];
				if(frontFace == "ccw")
					rasterizationState.setFrontFace(vk::FrontFace::eCounterClockwise);
				else if(frontFace == "cw")
					rasterizationState.setFrontFace(vk::FrontFace::eClockwise);
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
			rasterizationState.lineWidth = 1.0f;
			return rasterizationState;
		}

		vk::PipelineColorBlendStateCreateInfo parseBlendAttachmentState(JSON::JSON& json, Graphics::Pipeline& pipeline)
		{
			vk::PipelineColorBlendStateCreateInfo info;
			pipeline.colorBlendAttachmentStates = std::vector<vk::PipelineColorBlendAttachmentState>(1);
			{
				int value = json["color-write-mask"];
				vk::ColorComponentFlags writeMask = vk::ColorComponentFlagBits(value);
				bool blendEnable = json["blending"];
				pipeline.colorBlendAttachmentStates[0] = vk::PipelineColorBlendAttachmentState()
					.setColorWriteMask(writeMask)
					.setBlendEnable(blendEnable)
					.setSrcColorBlendFactor(vk::BlendFactor::eOne)
					.setDstColorBlendFactor(vk::BlendFactor::eOne)
					.setColorBlendOp(vk::BlendOp::eAdd)
					.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
					.setDstAlphaBlendFactor(vk::BlendFactor::eOne)
					.setAlphaBlendOp(vk::BlendOp::eAdd);
			}
			info.setAttachmentCount(1).setPAttachments(pipeline.colorBlendAttachmentStates.data());
			return info;
		}

		vk::PipelineViewportStateCreateInfo parseViewportState(JSON::JSON& json)
		{
			vk::PipelineViewportStateCreateInfo viewport;
			{
				int viewportCount = json["viewport-count"];
				int scissorCount = json["scissor-count"];
				viewport.setViewportCount(viewportCount).setScissorCount(scissorCount);
			}
			return viewport;
		}

		vk::PipelineDynamicStateCreateInfo parseDynamicStates(JSON::JSON& json, Graphics::Pipeline& pipeline)
		{
			std::vector<JSON::JSON> vectorJSON = json;
			vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
			for(const std::string& state : vectorJSON)
			{
				if(state == "viewport")
					pipeline.dynamicStates.push_back(vk::DynamicState(vk::DynamicState::eViewport));
				if(state == "scissor")
					pipeline.dynamicStates.push_back(vk::DynamicState(vk::DynamicState::eScissor));
			}
			dynamicStateCreateInfo.setDynamicStateCount(pipeline.dynamicStates.size()).setPDynamicStates(pipeline.dynamicStates.data());
			return dynamicStateCreateInfo;
		}

		vk::PipelineDepthStencilStateCreateInfo parseDepthAndStencilState(JSON::JSON & json)
		{
			vk::PipelineDepthStencilStateCreateInfo state;
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
				if(depthCompareOp == "less-or-equal")
					state.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
			}
			{
				bool depthBounds = json["depth-bound-test"];
				state.setDepthBoundsTestEnable(depthBounds);
			}
			{
				const std::string& failOp = json["fail-op"];
				if(failOp == "keep")
					state.back.setFailOp(vk::StencilOp::eKeep);
			}
			{
				const std::string& passOp = json["pass-op"];
				if(passOp == "keep")
					state.back.setPassOp(vk::StencilOp::eKeep);
			}
			{
				const std::string& compareOp = json["compare-op"];
				if(compareOp == "always")
					state.back.setCompareOp(vk::CompareOp::eAlways);
			}
			{
				bool stencilTest = json["stencil-test"];
				state.setStencilTestEnable(stencilTest);
			}
			{
				const std::string& front = json["front"];
				if(front == "back")
					state.setFront(state.back);
			}
			return state;
		}

		vk::PipelineMultisampleStateCreateInfo parseMultisamplingState(JSON::JSON & json)
		{
			vk::PipelineMultisampleStateCreateInfo state;
			{
				const std::string& sampleMask = json["sample-mask"];
				if(sampleMask == "null")
					state.setPSampleMask(nullptr);
			}
			{
				int rasterizationSamples = json["rasterization-samples"];
				if(rasterizationSamples == 1)
					state.setRasterizationSamples(vk::SampleCountFlagBits::e1);
			}
			return state;
		}

		std::vector<vk::PipelineShaderStageCreateInfo> parseShaderStages(JSON::JSON & json, const vk::Device& device)
		{
			std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(2);
			const std::string& vertexLocation = json["vertex"];
			const std::string& fragmentLocation = json["fragment"];

			// TODO fix this by loading metadata and retreiving proper info
			shaderStages[0]
				.setStage(vk::ShaderStageFlagBits::eVertex)
				.setModule(Shader::load(vertexLocation.c_str(), device))
				.setPName("main");

			// TODO fix this by loading metadata and retreiving proper info
			shaderStages[1]
				.setStage(vk::ShaderStageFlagBits::eFragment)
				.setModule(Shader::load(fragmentLocation.c_str(), device))
				.setPName("main");

			return shaderStages;
		}
	};
}
