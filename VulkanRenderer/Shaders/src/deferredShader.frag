#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 0) uniform sampler2D samplerPosition;
layout (set = 0, binding = 1) uniform sampler2D samplerNormal;
layout (set = 0, binding = 2) uniform sampler2D samplerColor;
layout (set = 0, binding = 3) uniform DeferredUniformBuffer
{
    vec4 mLightDirection;
    vec4 mLightColor;
} ubo;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    vec4 color = texture(samplerColor, inTexcoord);
    vec3 normal = texture(samplerNormal, inTexcoord).rgb;
    float diffuseFactor = clamp(dot(normal, -1.0 * ubo.mLightDirection.rgb), 0.0, 1.0);
    outFinalColor = diffuseFactor *  color;
}