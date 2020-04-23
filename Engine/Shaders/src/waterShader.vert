#version 450
#extension GL_ARB_separate_shader_objects : enable

#define WATER_DISPLACEMENT_SCALE 5.0

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

layout(set = 2, binding = 0) uniform WaterUniformBuffer
{
	vec4 mPosition;
	vec4 mDeepColor;
	vec4 mShallowColor;
	vec4 mWaveSpeedHeight;
} uboWater;

layout (set = 2, binding = 1) uniform sampler2D displacementMapSampler;


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexcoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexcoord;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out vec3 outBitangent;
layout(location = 5) out vec4 outShadowCoordinate;
layout(location = 6) out mat3 outTBN;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main()
{
	vec3 worldPos = (uboGeometry.mWorldMatrix * vec4(inPosition, 1.0)).xyz;
	worldPos.y += WATER_DISPLACEMENT_SCALE * texture(displacementMapSampler, inTexcoord).r;
	
	vec3 rightPos = vec3(worldPos.x + 2.2727, inPosition.y, worldPos.z);
	vec3 downPos = vec3(worldPos.x, inPosition.y, worldPos.z + 2.2727);
	
	vec2 rightUV = inTexcoord + vec2(0.0113635, 0.0);
	vec2 downUV = inTexcoord + vec2(0.0, 0.0113635);
	
	rightPos.y += WATER_DISPLACEMENT_SCALE * texture(displacementMapSampler, rightUV).r;
	downPos.y += WATER_DISPLACEMENT_SCALE * texture(displacementMapSampler, downUV).r;

	outNormal = -normalize(cross(rightPos - worldPos, downPos - worldPos ));
	
    gl_Position = uboGeometry.mWVP * vec4(worldPos, 1.0);
    
    outPosition = worldPos;    
    outTexcoord = inTexcoord;    
    //outNormal = normalize((uboGeometry.mNormalMatrix * vec4(inNormal, 0.0)).xyz);
    outTangent = normalize((uboGeometry.mNormalMatrix * vec4(inTangent, 0.0)).xyz);
    outBitangent = normalize(cross(outNormal, outTangent));
    outTBN = mat3(outTangent, outBitangent, outNormal);

	// Shadow map coordinate computation
	outShadowCoordinate = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}