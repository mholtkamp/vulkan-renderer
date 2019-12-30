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
#include "EnvironmentCapture.h"
#include "DirectionalLight.h"

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

	void RenderShadowCasters(VkCommandBuffer commandBuffer);

	void RenderLightVolumes(VkCommandBuffer commandBuffer);

	void Update(float deltaTime, bool updateDebug = true);

	Camera* GetActiveCamera();

	const std::string& GetDirectory() const;

	void SpawnTestLights();

	void SpawnTestEnvironmentCapture();

	void SetActiveCamera(Camera* activeCamera);

	void CaptureEnvironment();

    void UpdateShadowMapDescriptors();

	std::vector<EnvironmentCapture>& GetEnvironmentCaptures();

	DirectionalLight& GetDirectionalLight();

	TextureCube* GetIrradianceMap();

private:

	void AssignEnvironmentCaptures();

	void LoadMaterials(const aiScene& scene);

	void LoadMeshes(const aiScene& scene);

	void LoadActors(const aiScene& scene);

	void LoadEnvironmentCapture(const aiNode& node);

	void PopulateLightLookupMap(const aiScene& scene, 
		std::map<std::string, aiLight>& lightMap);

	void UpdateLightPositions(float deltaTime);

	void UpdateDebug(float deltaTime);

	void SetTestDirectionalLight();

private:

	std::string mDirectory;

	std::vector<Mesh> mMeshes;

	std::vector<Material> mMaterials;

	std::map<std::string, Texture2D> mTextures;

	std::vector<Actor> mActors;

	std::vector<PointLight> mPointLights;

	std::vector<Camera> mCameras;

	std::vector<EnvironmentCapture> mEnvironmentCaptures;

	DirectionalLight mDirectionalLight;

	Camera* mActiveCamera;

	bool mLoaded;

	bool mDebugMoveLights;
};