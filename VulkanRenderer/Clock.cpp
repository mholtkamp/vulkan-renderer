#include "Clock.h"

#include <chrono>

using namespace std;
using namespace std::chrono;

Clock::Clock()
{

}

Clock::~Clock()
{

}

void Clock::Start()
{
	mPreviousTime = high_resolution_clock::now();
	mCurrentTime = high_resolution_clock::now();
}

void Clock::Update()
{
	mCurrentTime = high_resolution_clock::now();
	mDeltaTime = duration_cast<milliseconds>(mCurrentTime - mPreviousTime).count() / 1000.0f;
	mPreviousTime = mCurrentTime;
}

float Clock::DeltaTime()
{
	return mDeltaTime;
}