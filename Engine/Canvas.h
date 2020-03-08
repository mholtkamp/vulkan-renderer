#pragma once

#include "Widget.h"

class Canvas : public Widget
{
public:

	Canvas();
	virtual ~Canvas();

	virtual void Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea) override;

};