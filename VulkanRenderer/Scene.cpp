#include "Scene.h"

Scene::Scene()
{

}

void Scene::Destroy()
{

}

void Scene::Load(const std::string& path)
{

}

void Scene::LoadMaterials(const aiScene& scene)
{
	int numMaterials = scene.mNumMaterials;

	mMaterials.reserve(numMaterials);

	for (int i = 0; i < numMaterials; ++i)
	{
		mMaterials[i].Create(*scene.mMaterials[i], mTextures);
	}
}

void Scene::LoadMeshes(const aiScene& scene)
{
	int numMeshes = scene.mNumMeshes;

	mMeshes.reserve(numMeshes);

	for (int i = 0; i < numMeshes; ++i)
	{
		mMeshes[i].Create(*scene.mMeshes[i], mMaterials);
	}
}

void Scene::LoadActors(const aiScene& scene)
{

}

void Scene::LoadPointLights(const aiScene& scene)
{

}