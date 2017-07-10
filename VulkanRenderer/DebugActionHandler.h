#pragma once

#include "Enums.h"
#include <stdint.h>

class DebugActionHandler
{

public:

	DebugActionHandler();

	~DebugActionHandler() = default;

	void Update();

private:

	bool mDebugMode;

	GBufferIndex mGBufferViewMode;

	uint32_t mEnvironmentCapture;
	uint32_t mEnvironmentCaptureFace;

};