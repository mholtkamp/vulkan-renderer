#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 0) uniform GlobalUniformBuffer
{
	mat4 mSunVP;
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
layout (set = 1, binding = 4) uniform sampler2D samplerMetallic;
layout (set = 1, binding = 5) uniform sampler2D samplerRoughness;
layout (set = 1, binding = 6) uniform sampler2D samplerShadowMap;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

const mat4 BIAS_MAT = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

const float AMBIENT_POWER = 0.2;

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

	// Determine if color should be shadowed
	float visibility = 1.0;
	float bias = 0.004f;

	vec4 shadowCoord = (BIAS_MAT * ubo.mSunVP) * vec4(position, 1.0);
	//shadowCoord = shadowCoord / shadowCoord.w;

	if (texture(samplerShadowMap, shadowCoord.xy).r + bias <  shadowCoord.z)
	{
		visibility = AMBIENT_POWER;
	}

	outFinalColor *= visibility;
}