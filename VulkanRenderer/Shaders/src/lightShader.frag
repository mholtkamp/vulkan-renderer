#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 1) uniform sampler2D samplerPosition;
layout (set = 0, binding = 2) uniform sampler2D samplerNormal;
layout (set = 0, binding = 3) uniform sampler2D samplerColor;

layout (set = 0, binding = 0) uniform DeferredUniformBuffer
{
    vec4 mSunDirection;
    vec4 mSunColor;
    vec2 mScreenDimensions;
} ubo;

layout(set = 1, binding = 0) uniform LightUniformBuffer
{
    mat4 mWVP;
    vec4 mPosition;
    vec4 mColor;
    float mConstantAttenuation;
    float mLinearAttenuation;
    float mQuadraticAttenuation;
} light;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    vec2 texcoord = gl_FragCoord.xy/ubo.mScreenDimensions;
    vec3 position = texture(samplerPosition, texcoord).rgb;
    vec3 normal = texture(samplerNormal, texcoord).rgb;
    vec4 color = texture(samplerColor, texcoord);
    
    //vec3 lightVector = -1.0 * normalize(ubo.mLightDirection.rgb);
    vec3 lightVector = normalize(light.mPosition.xyz - position);
    float diffuseFactor = clamp(dot(normal, lightVector), 0.0, 1.0);
    
    //// Output final, lit image.
    outFinalColor = diffuseFactor *  color ;
    
    outFinalColor = color;
    //outFinalColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}