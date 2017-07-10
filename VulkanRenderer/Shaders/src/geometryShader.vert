#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform GeometryUniformBuffer 
{
    mat4 mWVP;
    mat4 mWorldMatrix;
    mat4 mNormalMatrix;
    float mReflectivity;
} uboGeometry;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexcoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out vec3 outBitangent;
layout(location = 5) out mat3 outTBN;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = uboGeometry.mWVP * vec4(inPosition, 1.0);
    
    outPosition = (uboGeometry.mWorldMatrix * vec4(inPosition, 1.0)).xyz;    
    outTexcoord = inTexcoord;    
    outNormal = (uboGeometry.mNormalMatrix * vec4(inNormal, 0.0)).xyz;
    outTangent = (uboGeometry.mNormalMatrix * vec4(inTangent, 0.0)).xyz;
    outBitangent = cross(outNormal, outTangent);
    outTBN = mat3(outTangent, outBitangent, outNormal);   
}