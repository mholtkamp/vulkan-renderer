#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;

layout(set = 0, binding = 1) uniform sampler2D diffuseSampler;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;

void main()
{
    outPosition = vec4(inPosition, 1.0);
    outNormal = vec4(inNormal, 0.0);
    outColor = texture(diffuseSampler, inTexcoord);
    
    if (outColor.a < 0.5)
    {
        discard;
    }
}