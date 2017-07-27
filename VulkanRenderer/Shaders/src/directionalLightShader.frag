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
layout (set = 1, binding = 7) uniform samplerCube samplerIrradianceMap;

layout (location = 0) in vec2 inTexcoord;

layout (location = 0) out vec4 outFinalColor;

const mat4 BIAS_MAT = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

const float AMBIENT_POWER = 0.2;
const float BIAS = 0.004f;
const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float numer = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return numer / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float numer = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return numer / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float filterPCF(vec4 sc)
{
	ivec2 texDim = textureSize(samplerShadowMap, 0);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;
	
	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
        	if (texture(samplerShadowMap, sc.xy + vec2(dx*x, dy*y)).r + BIAS >=  sc.z)
            {
                shadowFactor += 1.0;
            }
    
			count++;
		}
	
	}
	
    return max(shadowFactor/count, AMBIENT_POWER);
}

void main()
{
    vec2 texcoord = gl_FragCoord.xy/ubo.mScreenDimensions;
    vec3 position = texture(samplerPosition, texcoord).rgb;
    vec3 normal = texture(samplerNormal, texcoord).rgb;
    vec3 albedo = texture(samplerColor, texcoord).rgb;
    vec4 specularColor = texture(samplerSpecularColor, texcoord);
	float metallic = texture(samplerMetallic, texcoord).r;
	float roughness = texture(samplerRoughness, texcoord).r;
    
    vec3 N = normalize(normal);
	vec3 V = normalize(ubo.mViewPosition.rgb - position);
	vec3 L = normalize(-ubo.mSunDirection.rgb);
	vec3 H = normalize(V + L);

	vec3 radiance = ubo.mSunColor.rgb;
    
    vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
    
    vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
    
    vec3 numerator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N, L), 0.0);
	vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
    
    vec3 ambient = texture(samplerIrradianceMap, N).rgb;

    // Output final, lit image.
    vec3 color = Lo + ambient;
    
    outFinalColor = vec4(color, 1.0);

	// Determine if color should be shadowed
	float visibility = 1.0;

	vec4 shadowCoord = (BIAS_MAT * ubo.mSunVP) * vec4(position, 1.0);
	shadowCoord = shadowCoord / shadowCoord.w;
    
    visibility = filterPCF(shadowCoord);
    
	outFinalColor *= visibility;
   
    outFinalColor = max(vec4(0.0), outFinalColor);
}
