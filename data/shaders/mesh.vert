#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (binding = 0) uniform UBO
{
	mat4 projectionMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;
} ubo;


layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUV;

void main() 
{
	outUV = inUV;

	vec3 lightPos = vec3(0.0, 5.0, 0.0);
	float lightDotNormal = dot(lightPos, inNormal);

	outColor = vec3(1.0, 1.0, 1.0) * lightDotNormal;
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * ubo.modelMatrix * vec4(inPos.xyz, 1.0);
}
