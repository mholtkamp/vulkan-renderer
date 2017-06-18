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

	GBufferIndex mGBufferViewMode;

};