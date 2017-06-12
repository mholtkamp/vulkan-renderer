#pragma once

#include <string>
#include <vector>
#include <map>

#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Actor.h"
#include "PointLight.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Scene
{
public:

	Scene();

	void Destroy();

	void Load(const std::string& path);

private:

	void LoadMaterials(const aiScene& scene);

	void LoadMeshes(const aiScene& scene);

	void LoadActors(const aiScene& scene);

	void LoadPointLights(const aiScene& scene);

private:

	std::vector<Mesh> mMeshes;

	std::vector<Material> mMaterials;

	std::map<std::string, Texture> mTextures;

	std::vector<Actor> mActors;

	std::vector<PointLight> mPointLights;

};