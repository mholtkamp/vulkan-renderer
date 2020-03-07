#pragma once

#include "Rect.h"
#include <vulkan/vulkan.h>
#include <vector>

class Widget
{
public:

	Widget();
	virtual ~Widget();

	virtual void Render(VkCommandBuffer commandBuffer, Rect area);

	void AddChild(Widget* widget);

	Widget* RemoveChild(Widget* widget);

	Widget* RemoveChild(int32_t index);

	Widget* GetChild(int32_t index);

protected:

	void SetScissor(VkCommandBuffer commandBuffer, Rect& area);

	Widget* mParent;
	std::vector<Widget*> mChildren;

	Rect mRect;
	bool mSetScissor;
};