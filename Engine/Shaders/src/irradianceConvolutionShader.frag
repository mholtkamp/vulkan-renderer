#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "common.glsl"

const float PI = 3.14159265359;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer 
{
	GlobalUniforms globals;
};

layout (set = 1, binding = 0) uniform samplerCube environmentSampler;

layout (set = 1, binding = 1) uniform IrradianceUniformBuffer
{
    mat4 mRotation;
} irr;

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

void main()
{
    vec3 irradiance = vec3(0.0);  
    vec3 normal = vec3(inPosition, 1.0);
    normal.y = -normal.y;
    normal = (irr.mRotation * vec4(normal, 0.0)).xyz;
    
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up         = cross(normal, right);

    float sampleDelta = 0.1; //0.025;
    float nrSamples = 0.0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

            irradiance += texture(environmentSampler, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
	
	outFinalColor = vec4(irradiance, 1.0);
}