#include "DebugActionHandler.h"
#include "Enums.h"
#include "Renderer.h"

#include <Windows.h>

DebugActionHandler::DebugActionHandler() :
	mGBufferViewMode(GB_COUNT),
	mEnvironmentCapture(0),
	mEnvironmentCaptureFace(0)
{

}

void DebugActionHandler::Update()
{
	GBufferIndex currentDebugView = mGBufferViewMode;

	if (GetAsyncKeyState(VK_CONTROL))
	{
		char numKey = '1';

		for (uint32_t i = 0; i < GB_COUNT; ++i)
		{
			if (GetAsyncKeyState(numKey))
			{
				mGBufferViewMode = static_cast<GBufferIndex>(i);
			}

			++numKey;
		}

		if (GetAsyncKeyState('0'))
		{
			// Disable debug view
			mGBufferViewMode = GB_COUNT;
		}
	}

	if (currentDebugView != mGBufferViewMode)
	{
		Renderer::Get()->SetVisualizationMode(mGBufferViewMode == GB_COUNT ? -1 : mGBufferViewMode);
	}


	if (GetAsyncKeyState(VK_CONTROL) &&
		GetAsyncKeyState(VK_TAB))
	{
		Renderer::Get()->SetDebugMode(DEBUG_GBUFFER);
	}

	if (GetAsyncKeyState(VK_CONTROL) &&
		GetAsyncKeyState(VK_SHIFT))
	{
		Renderer::Get()->SetDebugMode(DEBUG_NONE);
	}

	if (GetAsyncKeyState(VK_CONTROL) &&
		GetAsyncKeyState('F'))
	{
		Renderer::Get()->SetDebugMode(DEBUG_ENVIRONMENT_CAPTURE);
	}
}
