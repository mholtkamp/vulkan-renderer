#pragma once

#include <string>
#include <vector>
#include <map>

#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Actor.h"
#include "PointLight.h"
#include "Clock.h"
#include "Camera.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Scene
{
public:

	Scene();

	void Destroy();

	void Load(const std::string& directory,
		const std::string& file);

	void RenderGeometry(VkCommandBuffer commandBuffer);

	void RenderLightVolumes(VkCommandBuffer commandBuffer);

	void Update(float deltaTime);

	Camera* GetActiveCamera();

	const std::string& GetDirectory() const;

private:

	void LoadMaterials(const aiScene& scene);

	void LoadMeshes(const aiScene& scene);

	void LoadActors(const aiScene& scene);

	void PopulateLightLookupMap(const aiScene& scene, 
		std::map<std::string, aiLight>& lightMap);

private:

	std::string mDirectory;

	std::vector<Mesh> mMeshes;

	std::vector<Material> mMaterials;

	std::map<std::string, Texture> mTextures;

	std::vector<Actor> mActors;

	std::vector<PointLight> mPointLights;

	std::vector<Camera> mCameras;

	Camera* mActiveCamera;

	bool mLoaded;
};