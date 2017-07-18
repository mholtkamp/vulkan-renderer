#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 1, binding = 0) uniform sampler2D samplerPosition;
layout (set = 1, binding = 1) uniform sampler2D samplerNormal;
layout (set = 1, binding = 2) uniform sampler2D samplerColor;
layout (set = 1, binding = 3) uniform sampler2D samplerSpecularColor;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
	mat4 mSunVP;
    vec4 mSunDirection;
    vec4 mSunColor;
    vec4 mViewPosition;
    vec2 mScreenDimensions;
    int mVisualizationMode;
} ubo;

layout (set = 2, binding = 0) uniform sampler2D environmentFace;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    outFinalColor = texture(environmentFace, inTexcoord);
}