#include "DebugActionHandler.h"
#include "Enums.h"
#include "Renderer.h"
#include "Input.h"
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

	if (IsKeyDown(VKEY_CONTROL))
	{
		char numKey = '1';

		for (uint32_t i = 0; i < GB_COUNT; ++i)
		{
			if (IsKeyJustDown(numKey))
			{
				mGBufferViewMode = static_cast<GBufferIndex>(i);
			}

			++numKey;
		}

		if (IsKeyJustDown('0'))
		{
			// Disable debug view
			mGBufferViewMode = GB_COUNT;
		}
	}

	if (currentDebugView != mGBufferViewMode)
	{
		Renderer::Get()->SetVisualizationMode(mGBufferViewMode == GB_COUNT ? -1 : mGBufferViewMode);
	}


	if (IsKeyDown(VKEY_CONTROL) &&
		IsKeyJustDown(VKEY_TAB))
	{
		Renderer::Get()->SetDebugMode(DEBUG_GBUFFER);
	}

	if (IsKeyDown(VKEY_CONTROL) &&
		IsKeyJustDown(VKEY_SHIFT))
	{
		Renderer::Get()->SetDebugMode(DEBUG_NONE);
	}

	if (IsKeyDown(VKEY_CONTROL) &&
		IsKeyJustDown(VKEY_F))
	{
		Renderer::Get()->SetDebugMode(DEBUG_ENVIRONMENT_CAPTURE);
	}

	if (IsKeyDown(VKEY_CONTROL) &&
		IsKeyJustDown(VKEY_M))
	{
		Renderer::Get()->SetDebugMode(DEBUG_SHADOW_MAP);
	}

    if (IsKeyDown(VKEY_F) &&
		IsKeyJustDown(VKEY_1))
    {
        Renderer::Get()->SetEnvironmentDebugFace(0);
    }

    if (IsKeyDown(VKEY_F) &&
		IsKeyJustDown(VKEY_2))
    {
        Renderer::Get()->SetEnvironmentDebugFace(1);
    }

    if (IsKeyDown(VKEY_F) &&
		IsKeyJustDown(VKEY_3))
    {
        Renderer::Get()->SetEnvironmentDebugFace(2);
    }

    if (IsKeyDown(VKEY_F) &&
		IsKeyJustDown(VKEY_4))
    {
        Renderer::Get()->SetEnvironmentDebugFace(3);
    }

    if (IsKeyDown(VKEY_F) &&
		IsKeyJustDown(VKEY_5))
    {
        Renderer::Get()->SetEnvironmentDebugFace(4);
    }

    if (IsKeyDown(VKEY_F) &&
		IsKeyJustDown(VKEY_6))
    {
        Renderer::Get()->SetEnvironmentDebugFace(5);
    }

	if (IsKeyJustDown(VKEY_K) &&
		IsKeyDown(VKEY_CONTROL))
	{
		Renderer::Get()->ToggleEnvironmentCaptureDebug();
	}

	if (IsKeyJustDown(VKEY_L) &&
		IsKeyDown(VKEY_CONTROL))
	{
		Renderer::Get()->ToggleIrradianceDebug();
	}

    static bool eDown = false;

    if (IsKeyJustDown(VKEY_E) &&
        IsKeyDown(VKEY_CONTROL))
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
