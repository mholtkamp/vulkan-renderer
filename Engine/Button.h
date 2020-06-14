#pragma once

#include "Widget.h"

class Texture;
class Quad;
class Text;

typedef void(*HandlerFP)(class Button* button);

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

	virtual void Update() override;

	// These functions need to adjust quad/text widget pos/dim.
	virtual void SetPosition(float x, float y) override;
	virtual void SetDimensions(float width, float height) override;

	ButtonState GetState();
	virtual void SetState(ButtonState newState);

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

	virtual void OnPressed();
	virtual void OnHover();

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
	bool mAutoSizeText;
	glm::vec2 mTextPaddingRatio;

	HandlerFP mHoveredHandler;
	HandlerFP mPressedHandler;

	Quad* mQuad;
	Text* mText;
};