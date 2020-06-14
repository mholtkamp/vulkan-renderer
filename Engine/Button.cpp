#pragma once

#include "Button.h"
#include "Quad.h"
#include "Text.h"
#include "Input.h"
#include "Engine.h"
#include "Renderer.h"
#include "ApplicationState.h"

Button::Button() :
	mNormalTexture(nullptr),
	mHoveredTexture(nullptr),
	mPressedTexture(nullptr),
	mDisabledTexture(nullptr),
	mNormalColor({1, 1, 1, 1}),
	mHoveredColor({1, 1, 0.5f, 1}),
	mPressedColor({1, 1, 0, 1}),
	mDisabledColor({0.5f, 0.5f, 0.5f, 1}),
	mState(ButtonState::Normal),
	mStateColorChangeSpeed(5.0f),
	mUseTextStateColor(false),
	mUseQuadStateColor(true),
	mAutoSizeText(true),
	mTextPaddingRatio(glm::vec2(0.035f, 0.05f)),
	mHoveredHandler(nullptr),
	mPressedHandler(nullptr),
	mHandleMouseInput(true),
	mQuad(nullptr),
	mText(nullptr)

{
	mUseScissor = true;

	mQuad = new Quad();
	mText = new Text();

	AddChild(mQuad);
	AddChild(mText);

	// Default dimensions
	SetDimensions(100, 30);

	mQuad->SetColor({ 0.2f, 0.2f, 0.8f, 1.0f },
		{ 0.2f, 0.2f, 0.8f, 1.0f },
		{ 0.5f, 0.2f, 0.8f, 0.5f },
		{ 0.5f, 0.2f, 0.8f, 0.5f });
;}

Button::~Button()
{
	// Children are deleted automatically in ~Widget().
}

void Button::Update()
{
	Widget::Update();

	if (mHandleMouseInput && mState != ButtonState::Disabled)
	{
		const bool containsMouse = Widget::IsMouseInsideInterfaceRect(mAbsoluteRect);
		const bool mouseDown = IsButtonDown(VBUTTON_LEFT) || IsButtonDown(VBUTTON_RIGHT);
		const bool mouseJustDown = IsButtonJustDown(VBUTTON_LEFT) || IsButtonJustDown(VBUTTON_RIGHT);
		const bool mouseJustUp = IsButtonJustUp(VBUTTON_LEFT) || IsButtonJustUp(VBUTTON_RIGHT);

		if (containsMouse)
		{
			if (mouseJustUp &&
				mState == ButtonState::Pressed)
			{
				OnPressed();
			}
			else if (mouseJustDown)
			{
				SetState(ButtonState::Pressed);
			}
			else if (!mouseDown)
			{
				SetState(ButtonState::Hovered);
				OnHover();
			}
		}
		else
		{
			SetState(ButtonState::Normal);
		}
	}

	if (mDirty)
	{
		glm::vec4 stateColor = glm::vec4(1, 1, 1, 1);
		Texture* quadTexture = nullptr;

		switch (mState)
		{
		case ButtonState::Normal:
			quadTexture = mNormalTexture;
			stateColor = mNormalColor;
			break;
		case ButtonState::Hovered:
			quadTexture = mHoveredTexture;
			stateColor = mHoveredColor;
			break;
		case ButtonState::Pressed:
			quadTexture = mPressedTexture;
			stateColor = mPressedColor;
			break;
		case ButtonState::Disabled:
			quadTexture = mDisabledTexture;
			stateColor = mDisabledColor;
			break;
		}

		mQuad->SetTexture(quadTexture);

		if (mUseQuadStateColor)
		{
			mQuad->SetTint(stateColor);
		}

		if (mUseTextStateColor)
		{
			mText->SetColor(stateColor);
		}
	}
}

void Button::SetPosition(float x, float y)
{
	Widget::SetPosition(x, y);
	mQuad->SetPosition(0, 0);
}

void Button::SetDimensions(float width, float height)
{
	Widget::SetDimensions(width, height);
	mQuad->SetDimensions(width, height);
	mText->SetDimensions(width, height);

	if (mAutoSizeText)
	{
		const float textScaleFactor = 1.5f;
		const float textSize = textScaleFactor * height * (1 - 2 * mTextPaddingRatio.y);
		mText->SetSize(textSize);
	}

	mText->SetPosition(width * mTextPaddingRatio.x, height * mTextPaddingRatio.y);
}

ButtonState Button::GetState()
{
	return mState;
}

void Button::SetState(ButtonState newState)
{
	if (mState != newState)
	{
		mState = newState;
		MarkDirty();

		// If not handling mouse input, then call these when SetState is manually called.
		if (!mHandleMouseInput)
		{
			if (newState == ButtonState::Hovered && mHoveredHandler != nullptr)
			{
				OnHover();
			}
			else if (newState == ButtonState::Pressed && mPressedHandler != nullptr)
			{
				OnPressed();
			}
		}
	}
}

void Button::SetNormalTexture(Texture* texture)
{
	if (mNormalTexture != texture)
	{
		mNormalTexture = texture;
		MarkDirty();
	}
}

void Button::SetHoveredTexture(Texture* texture)
{
	if (mHoveredTexture != texture)
	{
		mHoveredTexture = texture;
		MarkDirty();
	}
}

void Button::SetPressedTexture(Texture* texture)
{
	if (mPressedTexture != texture)
	{
		mPressedTexture = texture;
		MarkDirty();
	}
}

void Button::SetDisabledTexture(Texture* texture)
{
	if (mDisabledTexture != texture)
	{
		mDisabledTexture = texture;
		MarkDirty();
	}
}

void Button::SetNormalColor(glm::vec4 color)
{
	if (mNormalColor != color)
	{
		mNormalColor = color;
		MarkDirty();
	}
}

void Button::SetHoveredColor(glm::vec4 color)
{
	if (mHoveredColor != color)
	{
		mHoveredColor = color;
		MarkDirty();
	}
}

void Button::SetPressedColor(glm::vec4 color)
{
	if (mPressedColor != color)
	{
		mPressedColor = color;
		MarkDirty();
	}
}

void Button::SetDisabledColor(glm::vec4 color)
{
	if (mDisabledColor != color)
	{
		mDisabledColor = color;
		MarkDirty();
	}
}

void Button::SetUseQuadStateColor(bool inUse)
{
	mUseQuadStateColor = inUse;
}

void Button::SetUseTextStateColor(bool inUse)
{
	mUseTextStateColor = inUse;
}

void Button::SetHandleMouseInput(bool inHandle)
{
	mHandleMouseInput = inHandle;
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

void Button::OnPressed()
{
	if (mPressedHandler)
	{
		mPressedHandler(this);
	}
}

void Button::OnHover()
{
	if (mHoveredHandler)
	{
		mHoveredHandler(this);
	}
}