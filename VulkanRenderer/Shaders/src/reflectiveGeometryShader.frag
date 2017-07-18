#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in vec4 inShadowCoordinate;
layout(location = 6) in mat3 inTBN;


layout(set = 1, binding = 0) uniform GeometryUniformBuffer 
{
    mat4 mWVP;
    mat4 mWorldMatrix;
    mat4 mNormalMatrix;
    mat4 mLightMVP;
    float mReflectivity;
} uboGeometry;

layout(set = 1, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 1, binding = 2) uniform sampler2D specularSampler;
layout(set = 1, binding = 3) uniform sampler2D normalSampler;
layout(set = 1, binding = 4) uniform samplerCube environmentSampler;
layout(set = 1, binding = 5) uniform sampler2D shadowMapSampler;

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
    vec4 mSunDirection;
    vec4 mSunColor;
    vec4 mViewPosition;
    vec2 mScreenDimensions;
    int mVisualizationMode;
} ubo;

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
    
    vec3 incident = normalize(inPosition - ubo.mViewPosition.xyz);
    vec3 reflection = reflect(incident, outNormal.xyz);
    vec4 environmentColor = vec4(texture(environmentSampler, reflection).rgb, 1.0);
    outColor = mix(outColor, environmentColor, uboGeometry.mReflectivity);
    
	// Determine if colors should be shadowed
	float visibility = 1.0;
	float bias = 0.004f;

	vec4 shadowCoord = inShadowCoordinate;// / inShadowCoordinate.w;

	if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		float dist = texture(shadowMapSampler, shadowCoord.st).r + bias;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z) 
		{
			visibility = 0.2;
		}
	}

	//if (texture(shadowMapSampler, shadowCoord.xy ).z  <  shadowCoord.z)
	//{
	//	visibility = 0.5;
	//	outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//	outSpecularColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//}

	outColor *= visibility;
	outSpecularColor *= visibility;
}