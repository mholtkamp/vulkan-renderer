#include "TextField.h"
#include "Engine.h"
#include "Clock.h"
#include "Input.h"
#include "Text.h"
#include "Quad.h"
#include "Renderer.h"

TextField* TextField::sSelectedTextField = nullptr;
float TextField::sCursorBlinkTime = 0.0f;
const float TextField::sCursorBlinkPeriod = 0.2f;

TextField::TextField() :
	mTextEditHandler(nullptr),
	mTextConfirmHandler(nullptr),
	mCursorQuad(nullptr),
	mMaxCharacters(64)
{
	mCursorQuad = new Quad();
	mCursorQuad->SetVisible(false);
	AddChild(mCursorQuad);
}

TextField::~TextField()
{

}

void TextField::Update()
{
	// If not the active text field, just update as if it was a button.
	if (mState != ButtonState::Pressed)
	{
		Button::Update();
		return;
	}

	if (sSelectedTextField != this)
	{
		SetSelectedTextField(this);
	}

	// Handle Blinky Cursor
	const Clock* clock = GetAppClock();
	float deltaTime = clock->DeltaTime();
	sCursorBlinkTime -= deltaTime;

	if (sCursorBlinkTime <= 0.0f)
	{
		mCursorQuad->SetDimensions(10, mAbsoluteRect.mHeight * 0.8f);
		mCursorQuad->SetPosition(10, mAbsoluteRect.mHeight * 0.1f);

		mCursorQuad->SetVisible(!mCursorQuad->IsVisible());
		sCursorBlinkTime = sCursorBlinkPeriod;
	}

	// Add/Remove text characters based on keyboard input
	const std::vector<int32_t>& pressedKeys = GetJustDownKeys();
	bool textStringModified = false;

	for (int32_t i = 0; i < pressedKeys.size(); ++i)
	{
		uint8_t keyCode = uint8_t(pressedKeys[i]);
		if (keyCode >= ' ' &&
			keyCode <= '~' &&
			mText->GetText().size() < mMaxCharacters)
		{
			uint8_t charToAdd = keyCode;
			const bool shiftDown = IsKeyDown(VKEY_SHIFT);

			if (charToAdd >= 'A' &&
				charToAdd <= 'Z' &&
				!shiftDown)
			{
				// If not shifted, make the character lower-case.
				// TODO: handle caps lock state.
				charToAdd += 32;
			}
			else if (shiftDown)
			{
				switch (charToAdd)
				{
				case '`': charToAdd = '~'; break;
				case '1': charToAdd = '!'; break;
				case '2': charToAdd = '@'; break;
				case '3': charToAdd = '#'; break;
				case '4': charToAdd = '$'; break;
				case '5': charToAdd = '%'; break;
				case '6': charToAdd = '^'; break;
				case '7': charToAdd = '&'; break;
				case '8': charToAdd = '*'; break;
				case '9': charToAdd = '('; break;
				case '0': charToAdd = ')'; break;
				case '-': charToAdd = '_'; break;
				case '=': charToAdd = '+'; break;
				case '[': charToAdd = '{'; break;
				case ']': charToAdd = '}'; break;
				case '\\': charToAdd = '|'; break;
				case ';': charToAdd = ':'; break;
				case '\'': charToAdd = '\"'; break;
				case ',': charToAdd = '<'; break;
				case '.': charToAdd = '>'; break;
				case '/': charToAdd = '?'; break;
				}
			}

			std::string newText = mText->GetText();
			newText += charToAdd;
			mText->SetText(newText);
			textStringModified = true;
		}

		// Remove characters
		// TODO: If cursor movement is added, make backspace remove characters behind cursor.
		//		and delete remove characters in front of the cursor.
		if (keyCode == VKEY_BACKSPACE||
			keyCode == VKEY_DELETE)
		{
			if (IsKeyDown(VKEY_CONTROL))
			{
				// For now, delete everything
				mText->SetText("");
			}
			else
			{
				std::string newText = mText->GetText();

				if (!newText.empty())
				{
					newText.pop_back();
				}

				mText->SetText(newText);
			}

			textStringModified = true;
		}
	}

	if (textStringModified)
	{
		mTextEditHandler(this);
	}

	if (IsKeyJustDown(VKEY_ENTER) || ((IsButtonJustUp(VBUTTON_LEFT) || IsButtonJustUp(VBUTTON_RIGHT)) &&
		!Widget::IsMouseInsideInterfaceRect(mAbsoluteRect)))
	{
		SetSelectedTextField(nullptr);
	}
}

void TextField::SetState(ButtonState newState)
{
	Button::SetState(newState);
	mCursorQuad->SetVisible(newState == ButtonState::Pressed);
}

void TextField::SetTextEditHandler(TextFieldHandlerFP handler)
{
	mTextEditHandler = handler;
}

void TextField::SetTextConfirmHandler(TextFieldHandlerFP handler)
{
	mTextConfirmHandler = handler;
}

void TextField::SetSelectedTextField(TextField * newField)
{
	if (sSelectedTextField != nullptr)
	{
		sSelectedTextField->SetState(ButtonState::Normal);
		sSelectedTextField->mTextConfirmHandler(sSelectedTextField);
	}

	sSelectedTextField = newField;
	sCursorBlinkTime = 0.0f;

	if (sSelectedTextField != nullptr)
	{
		sSelectedTextField->SetState(ButtonState::Pressed);
	}
}
