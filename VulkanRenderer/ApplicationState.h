#pragma once

#include "ApplicationInfo.h"

struct AppState
{
	HINSTANCE mConnection;
	HWND mWindow;
	POINT mMinSize;
	int mWindowWidth;
	int mWindowHeight;
	int mValidationError;
	bool mInCallback;
	uint32_t mEnabledExtensionCount;
	const char* mEnabledExtensions[MAX_ENABLED_EXTENSIONS];
	uint32_t mEnabledLayersCount;
	const char* mEnabledLayers[MAX_ENABLED_LAYERS];
	bool mValidate;

	AppState()
	{
		mConnection = nullptr;
		mWindow = nullptr;
		mWindowWidth = APP_WINDOW_WIDTH;
		mWindowHeight = APP_WINDOW_HEIGHT;
		mValidationError = 0;
		mInCallback = false;
		mEnabledExtensionCount = 0;
		memset(mEnabledExtensions, 0, sizeof(mEnabledExtensions));
		mEnabledLayersCount = 0;
		memset(mEnabledLayers, 0, sizeof(mEnabledLayers));
		mValidate = true;
	}
};