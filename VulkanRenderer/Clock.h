#pragma once

#include <chrono>

class Clock
{
public:

	Clock();

	~Clock();

	void Start();

	void Update();

	float DeltaTime();

private:
	
	std::chrono::high_resolution_clock::time_point mPreviousTime;
	std::chrono::high_resolution_clock::time_point mCurrentTime;

	float mDeltaTime;
};