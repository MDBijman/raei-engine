#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texture;

out gl_PerVertex {
	vec4 gl_Position; // will use gl_Position
};

layout(location = 0) out vec2 outTexture;

layout(binding = 0) uniform Camera {
	mat4 mvp;
};

void main() {
    gl_Position = mvp * vec4(position, 0.0, 1.0);
	outTexture = texture;
}