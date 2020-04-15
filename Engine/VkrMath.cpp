#include "VkrMath.h"
#include <time.h>
#include <stdlib.h>

bool VkrMath::mRandSeeded = false;

float VkrMath::RandRange(float min, float max)
{
	if (!mRandSeeded)
		SeedRand();

	float randFloat = rand() / static_cast<float>(RAND_MAX);
	float retRand = min + (max - min) * randFloat;

	return retRand;
}

int32_t VkrMath::RandRange(int32_t min, int32_t max)
{
	if (!mRandSeeded)
		SeedRand();

	int32_t retRand = min + rand() % (max - min);

	return retRand;
}

void VkrMath::SeedRand()
{
	// TODO
}