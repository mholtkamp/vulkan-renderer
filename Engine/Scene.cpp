#include "Scene.h"
#include "Camera.h"
#include "Constants.h"
#include "Renderer.h"
#include <map>

using namespace std;

static const int numLights = 100;
static const glm::vec3 minExtents(-80.0f, 1.0f, -40.0f);
static const glm::vec3 maxExtents(80.0f, 60.0f, 40.0f);
static const glm::vec3 ranges = maxExtents - minExtents;
static const float lightMultiplier = 5;
static const float minSpeed = 0.5f;
static const float maxSpeed = 8.0f;
static const float speedRange = maxSpeed - minSpeed;
static const float radiusGrowSpeed = 2.0f;

static bool spawnedTestLights = false;

Scene::Scene() :
	mLoaded(false),
	mDebugMoveLights(true)
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
			aiProcess_FlipUVs);

		if (scene == nullptr)
		{
			throw exception("Failed to open Collada file");
		}

		LoadMaterials(*scene);
		LoadMeshes(*scene);
		LoadActors(*scene);
		AssignEnvironmentCaptures();

		//SpawnTestLights();
		SpawnTestEnvironmentCapture();
		SetTestDirectionalLight();

		mLoaded = true;
	}
}

void Scene::SetTestDirectionalLight()
{
	float lightValue = 5.0f;
	mDirectionalLight.SetEnabled(true);
	mDirectionalLight.SetColor(glm::vec4(lightValue, 4.0f, 3.5f, 1.0f));
	mDirectionalLight.SetDirection(glm::normalize(glm::vec3(1.0f, -1.0f, 0.2f)));
}

TextureCube* Scene::GetIrradianceMap()
{
	if (mEnvironmentCaptures.size() > 0)
	{
		return mEnvironmentCaptures[0].GetIrradianceMap();
	}

	return nullptr;
}

void Scene::UpdateDebug(float deltaTime)
{
	// Change Radii
	if (GetAsyncKeyState('T'))
	{
		for (PointLight& light : mPointLights)
		{
			float radius = light.GetRadius();
			radius += deltaTime * radiusGrowSpeed;
			light.SetRadius(radius);
		}
	}

	if (GetAsyncKeyState('R'))
	{
		for (PointLight& light : mPointLights)
		{
			float radius = light.GetRadius();
			radius -= deltaTime * radiusGrowSpeed;
			light.SetRadius(radius);
		}
	}

	static bool cDown = false;
	if (GetAsyncKeyState('C') &&
		GetAsyncKeyState(VK_CONTROL))
	{
		if (!cDown)
		{
			CaptureEnvironment();
		}

		cDown = true;
	}
	else
	{
		cDown = false;
	}

	if ((GetAsyncKeyState('A') || GetAsyncKeyState('D')) &&
		GetAsyncKeyState(VK_CONTROL))
	{
		glm::vec3 dir = mDirectionalLight.GetDirection();
		float deltaDir = 0.3f * deltaTime;
		
		if (GetAsyncKeyState('D'))
		{
			deltaDir *= -1.0f;
		}

		dir.z += deltaDir;
		dir = glm::normalize(dir);
		mDirectionalLight.SetDirection(dir);
	}

	static bool sDown = false;
	if (GetAsyncKeyState('S') &&
		GetAsyncKeyState(VK_CONTROL))
	{
		if (!sDown)
		{
			bool castShadows = !mDirectionalLight.ShouldCastShadows();
			mDirectionalLight.SetCastShadows(castShadows);
		}

		sDown = true;
	}
	else
	{
		sDown = false;
	}

	static bool oDown = false;
	if (GetAsyncKeyState('O') &&
		GetAsyncKeyState(VK_CONTROL))
	{
		if (!oDown &&
			!spawnedTestLights)
		{
			spawnedTestLights = true;
			mPointLights.clear();
			SpawnTestLights();
		}

		oDown = true;
	}
	else
	{
		oDown = false;
	}

	static bool pDown = false;
	if (GetAsyncKeyState('P') &&
		GetAsyncKeyState(VK_CONTROL))
	{
		if (!pDown)
		{
			mDebugMoveLights = !mDebugMoveLights;
		}

		pDown = true;
	}
	else
	{
		pDown = false;
	}
}

void Scene::SpawnTestLights()
{
	for (uint32_t i = 0; i < numLights; ++i)
	{
		mPointLights.push_back(PointLight());
		PointLight& pointLight = mPointLights.back();

		glm::vec3 position;
		position.x = ((rand() / static_cast<float>(RAND_MAX)) * ranges.x + minExtents.x);
		position.y = ((rand() / static_cast<float>(RAND_MAX)) * ranges.y + minExtents.y);
		position.z = ((rand() / static_cast<float>(RAND_MAX)) * ranges.z + minExtents.z);

		glm::vec3 color;
		color.r = rand() / static_cast<float>(RAND_MAX);
		color.g = rand() / static_cast<float>(RAND_MAX);
		color.b = rand() / static_cast<float>(RAND_MAX);

		color = color * lightMultiplier;

		//color = glm::normalize(color);

		pointLight.Create(position, color, 8.0f);
		//pointLight.SetRadius(5.0f);

		glm::vec3 velocity;
		float speed = (rand() / static_cast<float>(RAND_MAX)) * minSpeed + speedRange;
		velocity.x = rand() / static_cast<float>(RAND_MAX);
		velocity.y = rand() / static_cast<float>(RAND_MAX);
		velocity.z = rand() / static_cast<float>(RAND_MAX);
		velocity = speed * glm::normalize(velocity);

		pointLight.SetVelocity(velocity);
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
		std::string actorName = nodes[i]->mName.C_Str();

		if (actorName.find("Capture") == 0)
		{
			EnvironmentCapture capture;

			aiMatrix4x4 transform = nodes[i]->mTransformation;
			capture.SetPosition(glm::vec3(transform.a4, transform.b4, transform.c4));
			capture.SetScene(this);
			mEnvironmentCaptures.push_back(capture);
		}
		else if (nodes[i]->mNumMeshes > 0)
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

			glm::vec3 lowColor = pointLight.GetColor();
			pointLight.SetColor(lowColor * 10.0f);
			pointLight.SetRadius(15.0f);
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

DirectionalLight& Scene::GetDirectionalLight()
{
	return mDirectionalLight;
}

void Scene::RenderGeometry(VkCommandBuffer commandBuffer)
{	
	for (Actor& actor : mActors)
	{
		actor.Draw(commandBuffer);
	}
}

void Scene::RenderShadowCasters(VkCommandBuffer commandBuffer)
{
	for (Actor& actor : mActors)
	{
		actor.Draw(commandBuffer);
	}
}

void Scene::RenderLightVolumes(VkCommandBuffer commandBuffer)
{
	if (mPointLights.size() > 0)
	{
		PointLight::BindSphereMeshBuffers(commandBuffer);

		for (PointLight& pointLight : mPointLights)
		{
			pointLight.Draw(commandBuffer);
		}
	}
}

void Scene::Update(float deltaTime, bool updateDebug)
{

	if (mActiveCamera != nullptr)
	{
		mActiveCamera->Update();
	}

    mDirectionalLight.Update();

	for (Actor& actor : mActors)
	{
		actor.Update(this,deltaTime);
	}

	for (PointLight& pointLight : mPointLights)
	{
		pointLight.Update(this, deltaTime);
	}

    if (updateDebug)
    {
        UpdateLightPositions(deltaTime);

        UpdateDebug(deltaTime);
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

void Scene::UpdateLightPositions(float deltaTime)
{
	if (mDebugMoveLights)
	{
		for (uint32_t i = 0; i < mPointLights.size(); ++i)
		{
			glm::vec3 velocity = mPointLights[i].GetVelocity();
			glm::vec3 position = mPointLights[i].GetPosition();

			position += velocity * deltaTime;

			mPointLights[i].SetPosition(position);

			if (position.x < minExtents.x)
				velocity.x = abs(velocity.x);
			if (position.y < minExtents.y)
				velocity.y = abs(velocity.y);
			if (position.z < minExtents.z)
				velocity.z = abs(velocity.z);
			if (position.x > maxExtents.x)
				velocity.x = -abs(velocity.x);
			if (position.y > maxExtents.y)
				velocity.y = -abs(velocity.y);
			if (position.z > maxExtents.z)
				velocity.z = -abs(velocity.z);

			mPointLights[i].SetVelocity(velocity);
		}
	}
}

void Scene::SetActiveCamera(Camera* activeCamera)
{
	mActiveCamera = activeCamera;
}

void Scene::CaptureEnvironment()
{
	for (EnvironmentCapture& capture : mEnvironmentCaptures)
	{
		capture.Capture();
	}

    for (Actor& actor : mActors)
    {
        actor.UpdateEnvironmentSampler();
    }
}

void Scene::LoadEnvironmentCapture(const aiNode& node)
{
	mEnvironmentCaptures.push_back(EnvironmentCapture());
	aiMatrix4x4 transform = node.mTransformation;
	mEnvironmentCaptures.back().SetPosition(glm::vec3(transform.a4, transform.b4, transform.c4));
	mEnvironmentCaptures.back().SetScene(this);
}

void Scene::SpawnTestEnvironmentCapture()
{
	EnvironmentCapture testCapture;
	testCapture.SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	mEnvironmentCaptures.push_back(testCapture);
	mEnvironmentCaptures.back().SetScene(this);
}

void Scene::AssignEnvironmentCaptures()
{
	for (Actor& actor : mActors)
	{
		EnvironmentCapture* targetCapture = nullptr;
		float targetDistance = 999.0f;

		for (EnvironmentCapture& capture : mEnvironmentCaptures)
		{
			if (targetCapture == nullptr)
			{
				targetCapture = &capture;
				targetDistance = glm::distance(actor.GetPosition(), capture.GetPosition());
			}
			else
			{
				float distance = glm::distance(actor.GetPosition(), capture.GetPosition());

				if (distance < targetDistance)
				{
					targetCapture = &capture;
					targetDistance = distance;
				}
			}
		}

		if (targetCapture != nullptr)
		{
			actor.SetEnvironmentCapture(targetCapture);
		}
	}
}

std::vector<EnvironmentCapture>& Scene::GetEnvironmentCaptures()
{
	return mEnvironmentCaptures;
}