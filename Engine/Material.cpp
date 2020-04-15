#include "Material.h"
#include "Texture2D.h"
#include "Renderer.h"
#include "Constants.h"

#include <assimp/scene.h>

using namespace std;

Material::Material() :
	mName("Material"),
	mDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
	mSpecularColor(1.0f, 1.0f, 1.0f, 1.0f),
	mReflectivity(0.0f),
    mMetallic(0.0f),
    mRoughness(0.0f)
{
	for (uint32_t i = 0; i < SLOT_COUNT; ++i)
	{
		mTextures[i] = nullptr;
	}
}

void Material::Destroy()
{

}

void Material::Create()
{
	Renderer* renderer = Renderer::Get();

	mName = "Material";
	mTextures[SLOT_DIFFUSE] = &renderer->mWhiteTexture;
	mTextures[SLOT_SPECULAR] = &renderer->mWhiteTexture;
	mTextures[SLOT_NORMALS] = &renderer->mWhiteTexture;
	mTextures[SLOT_ORM] = &renderer->mWhiteTexture;
}

void Material::Create(const Scene& scene,
					  const aiMaterial& material,
					  std::map<std::string, Texture2D>& textures)
{
	aiString name;
	if (material.Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
	{
		mName = name.C_Str();
	}

	aiColor3D diffuseColor;
	if (material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == aiReturn_SUCCESS)
	{
		mDiffuseColor.r = diffuseColor.r;
		mDiffuseColor.g = diffuseColor.g;
		mDiffuseColor.b = diffuseColor.b;
		mDiffuseColor.a = 1.0f;
	}

	aiColor3D specularColor;
	if (material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == aiReturn_SUCCESS)
	{
		mSpecularColor.r = specularColor.r;
		mSpecularColor.g = specularColor.g;
		mSpecularColor.b = specularColor.b;
		mSpecularColor.a = 1.0f;

		if (specularColor.r == 0.0f &&
			specularColor.g == 0.0f &&
			specularColor.b == 0.0f)
		{
			mReflectivity = 0.0f;
			mMetallic = 0.01f;
			mRoughness = 1.0f;
		}
		if (specularColor.r == 0.5f &&
			specularColor.g == 0.5f &&
			specularColor.b == 0.5f)
		{
			mReflectivity = 0.0f;
			mMetallic = 0.01f;
			mRoughness = 1.0f;
		}
		else
		{
			mReflectivity = specularColor.r;
			mMetallic = specularColor.g;
			mRoughness = specularColor.b;
		}
	}

	aiString diffuseTexture;
	if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		material.GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexture);
		SetTexture(scene, textures, mTextures[SLOT_DIFFUSE], diffuseTexture.C_Str());
	}
	else
	{
		SetDefaultTexture(scene, textures, mTextures[SLOT_DIFFUSE], DEFAULT_DIFFUSE_TEXTURE_NAME);
	}

	aiString specularTexture;
	if (material.GetTextureCount(aiTextureType_SPECULAR) > 0)
	{
		material.GetTexture(aiTextureType_SPECULAR, 0, &specularTexture);
		SetTexture(scene, textures, mTextures[SLOT_SPECULAR], specularTexture.C_Str());
	}
	else
	{
		SetDefaultTexture(scene, textures, mTextures[SLOT_SPECULAR], DEFAULT_SPECULAR_TEXTURE_NAME);
	}

	aiString ormTexture;
	if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0)
	{
		material.GetTexture(aiTextureType_EMISSIVE, 0, &ormTexture);
		SetTexture(scene, textures, mTextures[SLOT_ORM], ormTexture.C_Str());
	}
	else
	{
		SetDefaultTexture(scene, textures, mTextures[SLOT_ORM], DEFAULT_ORM_TEXTURE_NAME);
	}

	aiString normalTexture;
	if (material.GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		material.GetTexture(aiTextureType_NORMALS, 0, &normalTexture);
		SetTexture(scene, textures, mTextures[SLOT_NORMALS], normalTexture.C_Str());
	}
	else
	{
		SetDefaultTexture(scene, textures, mTextures[SLOT_NORMALS], DEFAULT_NORMAL_TEXTURE_NAME);
	}

	//aiString emissiveTexture;
	//if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0)
	//{
	//  material.GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTexture);
	//	SetTexture(textures, SlotEmissive, emissiveTexture.C_Str());
	//}
}

void Material::UpdateDescriptorSets(VkDescriptorSet descriptorSet)
{
    Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDescriptorImageInfo imageInfo[SLOT_COUNT] = {};
	VkWriteDescriptorSet descriptorWrite[SLOT_COUNT] = {};

	for (uint32_t i = 0; i < SLOT_COUNT; ++i)
	{
		if (mTextures[i] == nullptr)
		{
			continue;
		}

		imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo[i].imageView = (mTextures[i] != nullptr) ? mTextures[i]->GetImageView() : VK_NULL_HANDLE;
		imageInfo[i].sampler =(mTextures[i] != nullptr) ? mTextures[i]->GetSampler() : VK_NULL_HANDLE;

		descriptorWrite[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite[i].dstSet = descriptorSet;
		descriptorWrite[i].dstBinding = AD_TEXTURE_DIFFUSE + i;
		descriptorWrite[i].dstArrayElement = 0;
		descriptorWrite[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite[i].descriptorCount = 1;
		descriptorWrite[i].pImageInfo = &imageInfo[i];

		vkUpdateDescriptorSets(device, 1, &descriptorWrite[i], 0, nullptr);
	}
}

void Material::SetTexture(const Scene& scene,
	map<string, Texture2D>& textures,
	Texture2D*& texture,
	string name)
{
	if (textures.find(name) == textures.end())
	{
		textures.insert(pair<string, Texture2D>(name, Texture2D()));
		Texture2D& texEntry = textures[name];
		texEntry.Load(scene.GetDirectory() + name);
		texture = &texEntry;
	}
	else
	{
		texture = &textures[name];
	}
}

float Material::GetMetallic()
{
    return mMetallic;
}

float Material::GetRoughness()
{
    return mRoughness;
}

void Material::SetDefaultTexture(const Scene& scene,
	std::map<std::string, Texture2D>& textures,
	Texture2D*& texture,
	std::string name)
{
	if (textures.find(name) == textures.end())
	{
		textures.insert(pair<string, Texture2D>(name, Texture2D()));
		Texture2D& texEntry = textures[name];
		texEntry.Load(DEFAULT_TEXTURE_DIRECTORY_NAME + name);
		texture = &texEntry;
	}
	else
	{
		texture = &textures[name];
	}
}

float Material::GetReflectivity()
{
	return mReflectivity;
}

void Material::SetReflectivity(float reflectivity)
{
	mReflectivity = reflectivity;
}