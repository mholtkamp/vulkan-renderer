#include "Scene.h"
#include "Camera.h"
#include <map>

using namespace std;

Scene::Scene() : 
	mLoaded(false)
{
	// TODO: Active camera should point to one of the
	// cameras loaded from the .dae file.
	mActiveCamera = new Camera();
	mActiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
}

void Scene::Destroy()
{
	for (Actor& actor : mActors)
	{
		actor.Destroy();
	}

	delete mActiveCamera;
	mActiveCamera = nullptr;
}

void Scene::Load(const std::string& directory,
	const std::string& file)
{
	if (!mLoaded)
	{
		std::string path = directory + file;
		mDirectory = directory;

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.c_str(),
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		LoadMaterials(*scene);
		LoadMeshes(*scene);
		LoadActors(*scene);
		//LoadCameras(*scene);

		mLoaded = true;
	}
}

void Scene::LoadMaterials(const aiScene& scene)
{
	int numMaterials = scene.mNumMaterials;

	mMaterials.resize(numMaterials);

	for (int i = 0; i < numMaterials; ++i)
	{
		mMaterials[i].Create(*this, *scene.mMaterials[i], mTextures);
	}
}

void Scene::LoadMeshes(const aiScene& scene)
{
	int numMeshes = scene.mNumMeshes;

	mMeshes.resize(numMeshes);

	for (int i = 0; i < numMeshes; ++i)
	{
		mMeshes[i].Create(*scene.mMeshes[i], &mMaterials);
	}
}

void Scene::LoadActors(const aiScene& scene)
{
	int numNodes = scene.mRootNode->mNumChildren;
	mActors.reserve(numNodes);

	aiNode** nodes = scene.mRootNode->mChildren;

	map<string, aiLight> pointLightDescriptions;
	PopulateLightLookupMap(scene, pointLightDescriptions);

	for (int i = 0; i < numNodes; ++i)
	{
		if (nodes[i]->mNumMeshes > 0)
		{
			mActors.push_back(Actor());
			Actor& actor = mActors.back();
			actor.Create(*nodes[i], mMeshes);
		}
		else if (pointLightDescriptions.find(nodes[i]->mName.C_Str()) != pointLightDescriptions.end())
		{
			// A point light description with the same name was found,
			// So create a point light object that uses the light description for
			// details like attenuation and color, but uses the node for 
			// determining its transform.
			aiLight& lightDesc = pointLightDescriptions.find(nodes[i]->mName.C_Str())->second;
			aiMatrix4x4 transform = nodes[i]->mTransformation;

			mPointLights.push_back(PointLight());
			PointLight& pointLight = mPointLights.back();
			pointLight.Create(lightDesc, 
				glm::vec3(transform.a4, transform.b4, transform.c4));
		}
	}
}

void Scene::PopulateLightLookupMap(const aiScene& scene,
	std::map<std::string, aiLight>& lightMap)
{
	for (unsigned int i = 0; i < scene.mNumLights; ++i)
	{
		aiLight& light = *scene.mLights[i];
		lightMap[light.mName.C_Str()] = light;
	}
}

void Scene::RenderGeometry(VkCommandBuffer commandBuffer)
{	
	for (Actor& actor : mActors)
	{
		actor.Draw(commandBuffer);
	}
}

void Scene::RenderLightVolumes(VkCommandBuffer commandBuffer)
{
	PointLight::BindSphereMeshBuffers(commandBuffer);

	for (PointLight& pointLight : mPointLights)
	{
		pointLight.Draw(commandBuffer);
	}
}

void Scene::Update(float deltaTime)
{

	if (mActiveCamera != nullptr)
	{
		mActiveCamera->Update();
	}

	for (Actor& actor : mActors)
	{
		actor.Update(this,deltaTime);
	}

	for (PointLight& pointLight : mPointLights)
	{
		pointLight.Update(this, deltaTime);
	}
}

Camera* Scene::GetActiveCamera()
{
	return mActiveCamera;
}

const std::string& Scene::GetDirectory() const
{
	return mDirectory;
}