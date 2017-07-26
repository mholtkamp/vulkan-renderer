#pragma once

#define RENDERER_MAX_DESCRIPTOR_SETS 4096
#define RENDERER_MAX_UNIFORM_BUFFER_DESCRIPTORS 4096
#define RENDERER_MAX_SAMPLER_DESCRIPTORS 2048
#define MINIMUM_INTENSITY (5.0f / 256.0f)
#define INVERSE_MININUM_INTENSITY (1.0f / MINIMUM_INTENSITY)

#define TEST_LIGHT_RANGE 25.0f

#define DEFAULT_TEXTURE_DIRECTORY_NAME "Textures/"
#define DEFAULT_DIFFUSE_TEXTURE_NAME "DefaultDiffuseTexture.png"
#define DEFAULT_SPECULAR_TEXTURE_NAME "DefaultSpecularTexture.png"
#define DEFAULT_NORMAL_TEXTURE_NAME "DefaultNormalTexture.png"
#define DEFAULT_REFLECTIVE_TEXTURE_NAME "DefaultReflectiveTexture.png"
#define DEFAULT_EMISSIVE_TEXTURE_NAME "DefaultEmissiveTexture.png"

#define ENVIRONMENT_CAPTURE_TEXTURE_COUNT 6
#define DEFAULT_ENVIRONMENT_CAPTURE_RESOLUTION 512
#define ENVIRONMENT_CAPTURE_MAX_RESOLUTION 2048

#define SHADOW_MAP_RESOLUTION 2048
#define IRRADIANCE_RESOLUTION 32