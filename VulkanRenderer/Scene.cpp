#include "Scene.h"

Scene::Scene()
{
	mClock.Start();

	// TODO: Active camera should point to one of the
	// cameras loaded from the .dae file.
	mActiveCamera = new Camera();
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

void Scene::RenderGeometry(VkCommandBuffer commandBuffer)
{	
	for (Actor& actor : mActors)
	{
		actor.Draw(commandBuffer);
	}
}

void Scene::Update()
{
	mClock.Update();

	for (Actor& actor : mActors)
	{
		actor.Update(mClock->DeltaTime());
	}
}