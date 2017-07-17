#include "DebugActionHandler.h"
#include "Enums.h"
#include "Renderer.h"

#include <Windows.h>

DebugActionHandler::DebugActionHandler() :
	mGBufferViewMode(GB_COUNT),
	mEnvironmentCapture(0),
	mEnvironmentCaptureFace(0),
    mAlwaysCapture(false)
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

	if (GetAsyncKeyState(VK_CONTROL) &&
		GetAsyncKeyState('M'))
	{
		Renderer::Get()->SetDebugMode(DEBUG_SHADOW_MAP);
	}

    if (GetAsyncKeyState('F') &&
        GetAsyncKeyState('1'))
    {
        Renderer::Get()->SetEnvironmentDebugFace(0);
    }

    if (GetAsyncKeyState('F') &&
        GetAsyncKeyState('2'))
    {
        Renderer::Get()->SetEnvironmentDebugFace(1);
    }

    if (GetAsyncKeyState('F') &&
        GetAsyncKeyState('3'))
    {
        Renderer::Get()->SetEnvironmentDebugFace(2);
    }

    if (GetAsyncKeyState('F') &&
        GetAsyncKeyState('4'))
    {
        Renderer::Get()->SetEnvironmentDebugFace(3);
    }

    if (GetAsyncKeyState('F') &&
        GetAsyncKeyState('5'))
    {
        Renderer::Get()->SetEnvironmentDebugFace(4);
    }

    if (GetAsyncKeyState('F') &&
        GetAsyncKeyState('6'))
    {
        Renderer::Get()->SetEnvironmentDebugFace(5);
    }

    static bool eDown = false;

    if (GetAsyncKeyState('E') &&
        GetAsyncKeyState(VK_CONTROL))
    {
        if (!eDown)
        {
            mAlwaysCapture = !mAlwaysCapture;
        }

        eDown = true;
    }
    else
    {
        eDown = false;
    }

    if (mAlwaysCapture)
    {
        Renderer::Get()->UpdateEnvironmentCaptures();
    }
}
