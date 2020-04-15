#pragma once

#include <stdint.h>

class VkrMath
{
public:

	static float RandRange(float min, float max);
	static int32_t RandRange(int32_t min, int32_t max);

private:

	static void SeedRand();
	static bool mRandSeeded;
};