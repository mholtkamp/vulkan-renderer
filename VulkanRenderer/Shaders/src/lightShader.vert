#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexcoord;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec2 outTexcoord;

layout(set = 1, binding = 0) uniform LightUniformBuffer
{
    mat4 mWVP;
    vec4 mPosition;
    vec4 mColor;
    float mRadius;
    float mConstantAttenuation;
    float mLinearAttenuation;
    float mQuadraticAttenuation;
} light;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	gl_Position = light.mWVP * vec4(inPosition, 1.0f);
    vec2 texcoord = gl_Position.xy/gl_Position.w;
    texcoord = texcoord/2.0 + 0.5;
    outTexcoord = texcoord;
}
