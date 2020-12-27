#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;
layout(location = 0) in vec2 textureCoord;

layout(binding = 1) uniform sampler2D image;

void main() {
    outColor = vec4(texture(image, textureCoord).xyz, 0.0);
	//outColor = vec4(1.0, 0.0, 0.0, 1.0);
}