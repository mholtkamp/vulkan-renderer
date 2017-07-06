#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in mat3 inTBN;

layout(set = 0, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 0, binding = 2) uniform sampler2D specularSampler;
layout(set = 0, binding = 3) uniform sampler2D normalSampler;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec4 outSpecularColor;

void main()
{
    outPosition = vec4(inPosition, 1.0);
    outColor = texture(diffuseSampler, inTexcoord);
    outSpecularColor = texture(specularSampler, inTexcoord);
    
    vec3 normal = texture(normalSampler, inTexcoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    outNormal = vec4(normalize(inTBN * normal), 0.0);
    
    if (outColor.a < 0.5)
    {
        discard;
    }
}