#pragma once

#include "Rect.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

class Widget
{
public:

	Widget();
	virtual ~Widget();

	// Setup any resources required by the widget.
	virtual void Create();

	virtual void Destroy();

	// Issue gpu commands to display the widget.
	// Recursively render children.
	virtual void Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea);

	// Refresh any data used for rendering based on this widget's state. Use dirty flag.
	// Recursively update children.
	virtual void Update();

	Rect GetRect();

	virtual void SetPosition(glm::vec2 position);
	virtual void SetDimensions(glm::vec2 dimensions);
	virtual void SetRect(glm::vec2 position, glm::vec2 dimensions);
	virtual void SetRect(Rect rect);

	virtual void SetColor(glm::vec4 color);

	void AddChild(Widget* widget);

	Widget* RemoveChild(Widget* widget);

	Widget* RemoveChild(int32_t index);

	Widget* GetChild(int32_t index);

	void MarkDirty();

protected:

	void SetScissor(VkCommandBuffer commandBuffer, Rect& area);

	void RenderChildren(VkCommandBuffer commandBuffer, Rect area);
	void UpdateChildren();

	Widget* mParent;
	std::vector<Widget*> mChildren;

	Rect mRect;
	glm::vec4 mColor;
	bool mSetScissor;
	bool mDirty;
};