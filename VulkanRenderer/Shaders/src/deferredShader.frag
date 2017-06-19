#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 0) uniform sampler2D samplerPosition;
layout (set = 0, binding = 1) uniform sampler2D samplerNormal;
layout (set = 0, binding = 2) uniform sampler2D samplerColor;
layout (set = 0, binding = 3) uniform DeferredUniformBuffer
{
    vec4 mLightDirection;
    vec4 mLightColor;
    int mVisualizationMode;
} ubo;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    vec3 position = texture(samplerPosition, inTexcoord).rgb;
    vec3 normal = texture(samplerNormal, inTexcoord).rgb;
    vec4 color = texture(samplerColor, inTexcoord);
    
    vec3 lightVector = -1.0 * normalize(ubo.mLightDirection.rgb);
    float diffuseFactor = clamp(dot(normal, lightVector), 0.0, 1.0);
    
    if (ubo.mVisualizationMode == -1)
    {
        // Output final, lit image.
        outFinalColor = diffuseFactor *  color;
    }
    else if (ubo.mVisualizationMode == 0)
    {
        // POSITION
        outFinalColor = vec4(position, 1.0);
    }
    else if (ubo.mVisualizationMode == 1)
    {
        // NORMAL
        outFinalColor = vec4(normal, 0.0);
    }
    else if (ubo.mVisualizationMode == 2)
    {
        // COLOR
        outFinalColor = color;
    }
}