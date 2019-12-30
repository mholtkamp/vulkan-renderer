#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "common.glsl"

layout (set = 1, binding = 0) uniform sampler2D samplerPosition;
layout (set = 1, binding = 1) uniform sampler2D samplerNormal;
layout (set = 1, binding = 2) uniform sampler2D samplerColor;
layout (set = 1, binding = 3) uniform sampler2D samplerSpecularColor;
layout (set = 1, binding = 4) uniform sampler2D samplerMetallic;
layout (set = 1, binding = 5) uniform sampler2D samplerRoughness;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer 
{
	GlobalUniforms globals;
};

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    vec3 position = texture(samplerPosition, inTexcoord).rgb;
    vec3 normal = texture(samplerNormal, inTexcoord).rgb;
    vec4 color = texture(samplerColor, inTexcoord);
    vec4 specularColor = texture(samplerSpecularColor, inTexcoord);
    
    vec3 lightVector = -1.0 * normalize(globals.mSunDirection.rgb);
    float diffuseFactor = clamp(dot(normal, lightVector), 0.0, 1.0);
    
    vec3 viewVector = normalize(globals.mViewPosition.xyz - position);
    vec3 halfwayVector = normalize(lightVector + viewVector);
    float specularFactor = pow(max(dot(normal, halfwayVector), 0.0), 10.0);
    
    if (globals.mVisualizationMode == -1)
    {
        // Output final, lit image.
        outFinalColor = diffuseFactor *  color + specularFactor * specularColor;
    }
    else if (globals.mVisualizationMode == 0)
    {
        // POSITION
        outFinalColor = vec4(position, 1.0);
    }
    else if (globals.mVisualizationMode == 1)
    {
        // NORMAL
        outFinalColor = vec4(normal, 0.0);
    }
    else if (globals.mVisualizationMode == 2)
    {
        // COLOR
        outFinalColor = color;
    }
    else if (globals.mVisualizationMode == 3)
    {
        // SPECULAR
        outFinalColor = specularColor;
    }
}