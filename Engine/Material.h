#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <assimp/scene.h>
#include "Enums.h"

#include <vulkan/vulkan.h>

class Texture;
class Texture2D;

class Material
{
public:

	Material();

	void Destroy();

	void Create(const class Scene& scene,
				const aiMaterial& material,
				std::map<std::string, Texture2D>& textures);

	void UpdateDescriptorSets(VkDescriptorSet descriptorSet);

	float GetReflectivity();

    float GetMetallic();

    float GetRoughness();

	void SetReflectivity(float reflectivity);

private:

	void SetTexture(const class Scene& scene,
		std::map<std::string, Texture2D>& textures,
		Texture2D*& texture,
		std::string name);

	void SetDefaultTexture(const class Scene& scene,
		std::map<std::string, Texture2D>& textures,
		Texture2D*& texture,
		std::string name);

	std::string mName;

	glm::vec4 mDiffuseColor;
	glm::vec4 mSpecularColor;

	float mReflectivity;
    float mMetallic;
    float mRoughness;

	Texture2D* mTextures[SLOT_COUNT];
};