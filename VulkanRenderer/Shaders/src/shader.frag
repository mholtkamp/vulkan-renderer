#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inTexcoord;
layout(location = 1) in vec3 inNormal;

layout(set = 0, binding = 1) uniform sampler2D diffuseSampler;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(diffuseSampler, inTexcoord);
}