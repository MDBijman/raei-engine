#pragma once
#include "Modules/ECS/Component.h"
#include "Modules/Importers/PipelineImporter.h"
#include "Modules/Graphics/Graphics.h"

namespace Components
{
	class Pipeline : public ECS::Component
	{
	public:
		template<class ShaderType>
		Pipeline(const std::string& location, vk::RenderPass& renderpass, vk::PipelineCache& cache, vk::Device& device, ShaderType& shader)
		{
			// Create the pipeline layout that is used to generate the rendering pipelines that
			// are based on this descriptor set layout
			// In a more complex scenario you would have different pipeline layouts for different
			// descriptor set layouts that could be reused
			vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
			pipelineLayoutCreateInfo
				.setPSetLayouts(&shader.getUniforms().getDescriptorSetLayout())
				.setSetLayoutCount(1);
			vk::PipelineLayout layout = device.createPipelineLayout(pipelineLayoutCreateInfo);

			pipeline = Importers::Pipeline::load(location, layout, shader.getAttributes().getVertices().vi, renderpass, cache, device);
		}

		Graphics::Pipeline pipeline;
	};
}

