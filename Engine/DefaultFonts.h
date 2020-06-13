#pragma once

#include "Font.h"

class DefaultFonts
{
public:

	static void Create();
	static void Destroy();

	// Regular
	static Font sRoboto32;
	static Font sPressStart16;
	static Font sUbuntu32;

	// Distance Field
	static Font sRoboto32_DF;

	// Monospace
	static Font sRobotoMono24;
	static Font sUbuntuMono24;
};
