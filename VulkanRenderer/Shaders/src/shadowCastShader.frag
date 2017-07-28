#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
	mat4 mSunVP;
    vec4 mSunDirection;
    vec4 mSunColor;
    vec4 mViewPosition;
    vec2 mScreenDimensions;
    int mVisualizationMode;
} ubo;

layout(set = 1, binding = 1) uniform sampler2D diffuseSampler;

layout(location = 0) in vec2 inTexcoord;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(diffuseSampler, inTexcoord);

    if (outColor.a < 0.5)
    {
        discard;
    }
}