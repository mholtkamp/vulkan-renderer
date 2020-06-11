#pragma once

#include "Widget.h"

class Texture;
class Quad;
class Text;

typedef void(*HandlerFP)(void);

enum class ButtonState
{
	Normal,
	Hovered,
	Pressed,
	Disabled,
	Num
};

class Button : public Widget
{
public:

	Button();
	virtual ~Button();

	// Issue gpu commands to display the widget.
	// Recursively render children.
	virtual void Render(VkCommandBuffer commandBuffer) override;

	// Refresh any data used for rendering based on this widget's state. Use dirty flag.
	// Recursively update children.
	virtual void Update() override;

	// These functions need to adjust quad/text widget pos/dim.
	virtual void SetPosition(float x, float y) override;
	virtual void SetDimensions(float width, float height) override;

	ButtonState GetState();
	void SetState(ButtonState newState);

	void SetNormalTexture(Texture* texture);
	void SetHoveredTexture(Texture* texture);
	void SetPressedTexture(Texture* texture);
	void SetDisabledTexture(Texture* texture);

	void SetNormalColor(glm::vec4 color);
	void SetHoveredColor(glm::vec4 color);
	void SetPressedColor(glm::vec4 color);
	void SetDisabledColor(glm::vec4 color);

	void SetUseQuadStateColor(bool inUse);
	void SetUseTextStateColor(bool inUse);
	void SetHandleMouseInput(bool inHandle);

	void SetHoverHandler(HandlerFP newHandler);
	void SetPressedHandler(HandlerFP newHandler);

	void SetTextString(const std::string& newTextString);
	const std::string& GetTextString() const;

	Text* GetText();
	Quad* GetQuad();

protected:

	Texture* mNormalTexture;
	Texture* mHoveredTexture;
	Texture* mPressedTexture;
	Texture* mDisabledTexture;

	glm::vec4 mNormalColor;
	glm::vec4 mHoveredColor;
	glm::vec4 mPressedColor;
	glm::vec4 mDisabledColor;

	ButtonState mState;
	float mStateColorChangeSpeed;

	bool mUseTextStateColor;
	bool mUseQuadStateColor;
	bool mHandleMouseInput;

	HandlerFP mHoveredHandler;
	HandlerFP mPressedHandler;

	Quad* mQuad;
	Text* mText;
};