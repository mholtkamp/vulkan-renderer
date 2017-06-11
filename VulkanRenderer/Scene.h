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

	std::vector<Mesh> mMeshes;

	std::vector<Material> mMaterials;

	std::map<std::string, Texture> mTextures;

	std::vector<Actor> mActors;

	std::vector<PointLight> mPointLights;

};