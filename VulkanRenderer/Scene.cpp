#include "Scene.h"
#include "Camera.h"

Scene::Scene() : 
	mLoaded(false)
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
		//LoadLights(*scene);
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
		mMeshes[i].Create(*scene.mMeshes[i], mMaterials);
	}
}

void Scene::LoadActors(const aiScene& scene)
{
	int numNodes = scene.mRootNode->mNumChildren;
	mActors.reserve(numNodes);

	aiNode** nodes = scene.mRootNode->mChildren;

	for (int i = 0; i < numNodes; ++i)
	{
		if (nodes[i]->mNumMeshes > 0)
		{
			mActors.push_back(Actor());
			Actor& actor = mActors.back();
			actor.Create(*nodes[i], mMeshes);
		}
	}
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

	if (mActiveCamera != nullptr)
	{
		mActiveCamera->Update();
	}

	for (Actor& actor : mActors)
	{
		actor.Update(this, mClock.DeltaTime());
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