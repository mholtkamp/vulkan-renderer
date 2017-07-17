#pragma once

#define INDEX(x) static_cast<uint32_t>(x)

enum TextureSlot
{
	SLOT_DIFFUSE,
	SLOT_SPECULAR,
	SLOT_NORMALS,
	SLOT_ENVIRONMENT,
	//SLOT_EMISSIVE,
	SLOT_COUNT
};

enum ActorDescriptor
{
	AD_UNIFORM_BUFFER,
	AD_TEXTURE_DIFFUSE,
	AD_TEXTURE_SPECULAR,
	AD_TEXTURE_NORMALS,
	AD_TEXTURE_ENVIRONMENT,
	//AD_TEXTURE_EMISSVE,
	AD_COUNT
};

enum DeferredDescriptor
{
	DD_TEXTURE_POSITION,
	DD_TEXTURE_NORMAL,
	DD_TEXTURE_COLOR,
	DD_TEXTURE_SPECULAR,
	DD_COUNT
};

enum LightDescriptor
{
	LD_UNIFORM_BUFFER,
	LD_COUNT
};

enum LightPassSetIndices
{
	LPS_GBUFFER_TEXTURES,
	LPS_LIGHT_DATA,
	LPS_COUNT
};

enum GBufferIndex
{
	GB_POSITION,
	GB_NORMAL,
	GB_COLOR,
	GB_SPECULAR,
	GB_COUNT
};

enum Attachments
{
	ATTACHMENT_BACK = 0,
	ATTACHMENT_DEPTH = 1,
	ATTACHMENT_GBUFFER_POSITION = 2,
	ATTACHMENT_GBUFFER_NORMAL = 3,
	ATTACHMENT_GBUFFER_COLOR = 4,
	ATTACHMENT_GBUFFER_SPECULAR = 5,
	ATTACHMENT_COUNT = 5
};

enum SubPasses
{
	PASS_DEPTH = 0,
	PASS_GEOMETRY = 1,
	PASS_DEFERRED = 2
};

enum class ProjectionMode
{
	ORTHOGRAPHIC,
	PERSPECTIVE
};

enum DebugMode
{
	DEBUG_NONE,
	DEBUG_GBUFFER,
	DEBUG_ENVIRONMENT_CAPTURE,
	DEBUG_SHADOW_MAP
};