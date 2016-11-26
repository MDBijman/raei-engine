#version 420

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

layout (binding = 1) uniform sampler2D imageTexture;

layout (location = 0) out vec4 outFragColor;

void main() 
{
  outFragColor = texture(imageTexture, inUV) * vec4(inColor, 1.0);
}