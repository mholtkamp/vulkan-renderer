#pragma once

#define INDEX(x) static_cast<uint32_t>(x)

enum TextureSlot
{
	SLOT_DIFFUSE,
	//SlotSpecular,
	//SlotNormals,
	//SlotEmissive,
	SLOT_COUNT
};

enum ActorDescriptor
{
	AD_UNIFORM_BUFFER,
	AD_TEXTURE_DIFFUSE,
	//AD_TEXTURE_SPECULAR,
	//AD_TEXTURE_NORMALS,
	//AD_TEXTURE_EMISSVE,
	AD_COUNT
};

enum GBufferIndex
{
	GB_POSITION,
	GB_NORMAL,
	GB_COLOR,
	GB_COUNT
};

enum Attachments
{
	ATTACHMENT_BACK = 0,
	ATTACHMENT_DEPTH = 1,
	ATTACHMENT_GBUFFER_POSITION = 2,
	ATTACHMENT_GBUFFER_NORMAL = 3,
	ATTACHMENT_GBUFFER_COLOR = 4,
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