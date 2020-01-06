#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "common.glsl"

layout(location = 0) in vec2 inTexcoord;

layout(set = 1, binding = 0) uniform GeometryUniformBuffer 
{
    mat4 mWVP;
    mat4 mWorldMatrix;
    mat4 mNormalMatrix;
    mat4 mLightMVP;
    float mReflectivity;
	float mMetallic;
	float mRoughness;
} uboGeometry;

layout(set = 1, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 2) uniform sampler2D specularSampler;
layout(set = 1, binding = 3) uniform sampler2D normalSampler;
layout(set = 1, binding = 4) uniform samplerCube environmentSampler;
layout(set = 1, binding = 5) uniform sampler2D shadowMapSampler;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer 
{
	GlobalUniforms globals;
};


void main()
{
    vec4 color = texture(diffuseSampler, inTexcoord);

    if (color.a < 0.5)
    {
        discard;
    }
}