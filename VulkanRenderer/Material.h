#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <assimp/scene.h>
#include "Enums.h"

#include <vulkan/vulkan.h>

class Texture;

class Material
{
public:

	Material();

	void Destroy();

	void Create(const class Scene& scene,
				const aiMaterial& material,
				std::map<std::string, Texture>& textures);

	void UpdateDescriptorSets(VkDescriptorSet descriptorSet);

private:

	void SetTexture(const class Scene& scene,
		std::map<std::string, Texture>& textures,
		Texture*& texture,
		std::string name);

	void SetDefaultTexture(const class Scene& scene,
		std::map<std::string, Texture>& textures,
		Texture*& texture,
		std::string name);

	std::string mName;

	glm::vec4 mDiffuseColor;
	glm::vec4 mSpecularColor;

	Texture* mTextures[SLOT_COUNT];
};