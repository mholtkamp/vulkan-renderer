#pragma once

#include "Widget.h"

class Texture;
class Quad;
class Text;

class Button : public Widget
{
	Widget();
	virtual ~Widget();

	// Setup any resources required by the widget.
	virtual void Create() override;

	virtual void Destroy() override;

	// Issue gpu commands to display the widget.
	// Recursively render children.
	virtual void Render(VkCommandBuffer commandBuffer) override;

	// Refresh any data used for rendering based on this widget's state. Use dirty flag.
	// Recursively update children.
	virtual void Update() override;

	Text* GetText();
	Quad* GetQuad();

protected:

	Texture* mNormalTexture;
	Texture* mHoverTexture;
	Texture* mPressTexture;

	glm::vec4 mNormalColor;
	glm::vec4 mHoverColor;
	glm::vec4 mPressColor;

	bool mUseTextStateColor;
	bool mUseQuadStateColor;

	Quad* mQuad;
	Text* mText;
};