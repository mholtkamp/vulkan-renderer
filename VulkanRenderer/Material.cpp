#include "Material.h"
#include "Texture.h"
#include "Renderer.h"

#include <assimp/scene.h>

using namespace std;

Material::Material() :
	mName("Material"),
	mDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
	mSpecularColor(1.0f, 1.0f, 1.0f, 1.0f)
{
	for (uint32_t i = 0; i < SLOT_COUNT; ++i)
	{
		mTextures[i] = nullptr;
	}
}

void Material::Destroy()
{

}

void Material::Create(const Scene& scene,
					  const aiMaterial& material,
					  std::map<std::string, Texture>& textures)
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
	}

	aiString diffuseTexture;
	if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		material.GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexture);
		SetTexture(scene, textures, mTextures[SLOT_DIFFUSE], diffuseTexture.C_Str());
	}

	//aiString specularTexture;
	//if (material.GetTextureCount(aiTextureType_SPECULAR) > 0)
	//{
	//  material.GetTexture(aiTextureType_SPECULAR, 0, &specularTexture);
	//	SetTexture(textures, SlotSpecular, specularTexture.C_Str());
	//}

	//aiString normalTexture;
	//if (material.GetTextureCount(aiTextureType_NORMALS) > 0)
	//{
	//  material.GetTexture(aiTextureType_NORMALS, 0, &normalTexture);
	//	SetTexture(textures, SlotNormals, normalTexture.C_Str());
	//}

	//aiString emissiveTexture;
	//if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0)
	//{
	//  material.GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTexture);
	//	SetTexture(textures, SlotEmissive, emissiveTexture.C_Str());
	//}
}

void Material::UpdateDescriptorSets(VkDescriptorSet descriptorSet)
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkDescriptorImageInfo imageInfo[SLOT_COUNT];
	VkWriteDescriptorSet descriptorWrite[SLOT_COUNT];

	for (uint32_t i = 0; i < SLOT_COUNT; ++i)
	{
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
	}

	vkUpdateDescriptorSets(device, SLOT_COUNT, descriptorWrite, 0, nullptr);
}

void Material::SetTexture(const Scene& scene,
	map<string, Texture>& textures,
	Texture*& texture,
	string name)
{
	if (textures.find(name) == textures.end())
	{
		textures.insert(pair<string, Texture>(name, Texture()));
		Texture& texEntry = textures[name];
		texEntry.Load(scene.GetDirectory() + name);
		texture = &texEntry;
	}
	else
	{
		texture = &textures[name];
	}
}