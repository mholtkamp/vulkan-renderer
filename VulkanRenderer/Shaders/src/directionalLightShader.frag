#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
    vec4 mSunDirection;
    vec4 mSunColor;
    vec4 mViewPosition;
    vec2 mScreenDimensions;
    int mVisualizationMode;
} ubo;

layout (set = 1, binding = 0) uniform sampler2D samplerPosition;
layout (set = 1, binding = 1) uniform sampler2D samplerNormal;
layout (set = 1, binding = 2) uniform sampler2D samplerColor;
layout (set = 1, binding = 3) uniform sampler2D samplerSpecularColor;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    vec2 texcoord = gl_FragCoord.xy/ubo.mScreenDimensions;
    vec3 position = texture(samplerPosition, texcoord).rgb;
    vec3 normal = texture(samplerNormal, texcoord).rgb;
    vec4 color = texture(samplerColor, texcoord);
    vec4 specularColor = texture(samplerSpecularColor, texcoord);
    
    // Diffuse Factor
    vec3 lightVector = -1.0 * normalize(ubo.mSunDirection.rgb);
    float diffuseFactor = clamp(dot(normal, lightVector), 0.0, 1.0);
    
    // Specular Factor
    vec3 viewVector = normalize(ubo.mViewPosition.xyz - position);
    vec3 halfwayVector = normalize(lightVector + viewVector);
    float specularFactor = pow(max(dot(normal, halfwayVector), 0.0), 10.0);
    
    // Output final, lit image.
    outFinalColor = (diffuseFactor * color * ubo.mSunColor) + (specularFactor * specularColor * ubo.mSunColor);
}