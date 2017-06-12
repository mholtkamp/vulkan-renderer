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

enum class ProjectionMode
{
	ORTHOGRAPHIC,
	PERSPECTIVE
};