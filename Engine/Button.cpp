#pragma once

#include "Button.h"
#include "Quad.h"
#include "Text.h"

Button::Button() :
	mNormalTexture(nullptr),
	mHoveredTexture(nullptr),
	mPressedTexture(nullptr),
	mDisabledTexture(nullptr),
	mNormalColor({1, 1, 1, 1}),
	mHoveredColor({1, 1, 0.5f, 1}),
	mPressedColor({1, 1, 0.2f, 1}),
	mDisabledColor({0.5f, 0.5f, 0.5f, 1}),
	mState(ButtonState::Normal),
	mStateColorChangeSpeed(5.0f),
	mUseTextStateColor(false),
	mUseQuadStateColor(true),
	mHoveredHandler(nullptr),
	mPressedHandler(nullptr),
	mQuad(nullptr),
	mText(nullptr)

{
	mQuad = new Quad();
	mText = new Text();

	AddChild(mQuad);
	AddChild(mText);
}

Button::~Button()
{
	delete mQuad;
	mQuad = nullptr;

	delete mText;
	mText = nullptr;
}

void Button::Render(VkCommandBuffer commandBuffer)
{
	// set scissor
	PushScissor(commandBuffer);

	// render
	RenderChildren(commandBuffer);

	// restore scissor
	PopScissor(commandBuffer);
}

void Button::Update()
{
	Widget::Update();
}

void Button::SetPosition(float x, float y)
{
	Widget::SetPosition(x, y);
	mQuad->SetPosition(x, y);
	mText->SetPosition(x, y);
}

void Button::SetDimensions(float width, float height)
{
	Widget::SetDimensions(width, height);
	mQuad->SetDimensions(width, height);
	mText->SetDimensions(width, height);
}

ButtonState Button::GetState()
{
	return mState;
}

void Button::SetState(ButtonState newState)
{
	mState = newState;
}

void Button::SetNormalTexture(Texture* texture)
{
	mNormalTexture = texture;
}

void Button::SetHoveredTexture(Texture* texture)
{
	mHoveredTexture = texture;
}

void Button::SetPressedTexture(Texture* texture)
{
	mPressedTexture = texture;
}

void Button::SetDisabledTexture(Texture* texture)
{
	mDisabledTexture = texture;
}

void Button::SetNormalColor(glm::vec4 color)
{
	mNormalColor = color;
}

void Button::SetHoveredColor(glm::vec4 color)
{
	mHoveredColor = color;
}

void Button::SetPressedColor(glm::vec4 color)
{
	mPressedColor = color;
}

void Button::SetDisabledColor(glm::vec4 color)
{
	mDisabledColor = color;
}

void Button::SetHoverHandler(HandlerFP newHandler)
{
	mHoveredHandler = newHandler;
}

void Button::SetPressedHandler(HandlerFP newHandler)
{
	mPressedHandler = newHandler;
}

void Button::SetTextString(const std::string& newTextString)
{
	mText->SetText(newTextString);
}

const std::string& Button::GetTextString() const
{
	return mText->GetText();
}

Text* Button::GetText()
{
	return mText;
}

Quad* Button::GetQuad()
{
	return mQuad;
}
