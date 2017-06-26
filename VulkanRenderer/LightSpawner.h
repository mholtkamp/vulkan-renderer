#pragma once

#include "Scene.h"
#include <glm/glm.hpp>

struct LightSpawnStats
{
	glm::vec3 mExtents;
	float mMinSpeed;
	float mMaxSpeed;

	LightSpawnStats() :
		mExtents(25.0f, 25.0f, 25.0f),
		mMinSpeed(0.5f),
		mMaxSpeed(2.0f)
	{
	}
};

class LightSpawner
{
public:

	LightSpawner();

	~LightSpawner();

	void SetSpawnStats(LightSpawnStats stats);

	void Update(float deltaTime);

	void SpawnLights(Scene& scene, uint32_t count);

private:

	std::vector<PointLight*> mLights;

	LightSpawnStats mStats;
};