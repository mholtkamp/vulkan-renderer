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
	virtual void Render(VkCommandBuffer commandBuffer);

	// Refresh any data used for rendering based on this widget's state. Use dirty flag.
	// Recursively update children.
	virtual void Update();

	Rect GetRect();
	Rect GetAbsoluteRect();

	virtual void SetPosition(float x, float y);
	virtual void SetDimensions(float width, float height);
	void SetPosition(glm::vec2 position);
	void SetDimensions(glm::vec2 dimensions);
	void SetRect(float x, float y, float width, float height);
	void SetRect(glm::vec2 position, glm::vec2 dimensions);
	void SetRect(Rect rect);

	virtual void SetColor(glm::vec4 color);

	void AddChild(Widget* widget);

	Widget* RemoveChild(Widget* widget);

	Widget* RemoveChild(int32_t index);

	Widget* GetChild(int32_t index);

	void MarkDirty();

	static float InterfaceToNormalized(float interfaceCoord, float interfaceSize);

protected:

	void SetScissor(VkCommandBuffer commandBuffer, Rect& area);

	void PushScissor(VkCommandBuffer commandBuffer);
	void PopScissor(VkCommandBuffer commandBuffer);

	void RenderChildren(VkCommandBuffer commandBuffer);
	void UpdateChildren();

	Widget* mParent;
	std::vector<Widget*> mChildren;

	Rect mRect; // Rect relative to parent
	Rect mAbsoluteRect; // Holds the true screen-space rect that is calculated on Update if dirty.
	glm::vec4 mColor;
	bool mSetScissor;
	bool mDirty;
};