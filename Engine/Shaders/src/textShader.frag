#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "common.glsl"

layout (set = 0, binding = 0) uniform GlobalUniformBuffer 
{
	GlobalUniforms globals;
};

layout (set = 1, binding = 0) uniform TextUniformBuffer
{	
	float mX;
	float mY;
	float mCutoff;
	float mOutlineSize;

	float mSize;
	float mPadding1;
	float mPadding2;
	float mPadding3;

	int mDistanceField;
	int mEffect;
} textData;
layout (set = 1, binding = 1) uniform sampler2D fontSampler;

layout (location = 0) in vec2 inTexcoord;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outFinalColor;

void main()
{
	float fontColor = texture(fontSampler, inTexcoord).r;
	outFinalColor = fontColor * inColor;
}