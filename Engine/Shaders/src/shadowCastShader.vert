#version 450
#extension GL_ARB_separate_shader_objects : enable

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

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;

layout(location = 0) out vec2 outTexcoord;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = uboGeometry.mLightMVP * vec4(inPosition, 1.0);
    outTexcoord = inTexcoord;
}