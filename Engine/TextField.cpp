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
	const bool shiftDown = IsKeyDown(VKEY_SHIFT);

	for (int32_t i = 0; i < pressedKeys.size(); ++i)
	{
		uint8_t keyCode = uint8_t(pressedKeys[i]);
		uint8_t charToAdd = ConvertKeyCodeToChar(keyCode, shiftDown);

		if (charToAdd >= ' ' &&
			charToAdd <= '~' &&
			mText->GetText().size() < mMaxCharacters)
		{
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

uint8_t TextField::ConvertKeyCodeToChar(uint8_t keyCode, bool shiftDown)
{
	uint8_t retChar = 0;

	const bool isLetter = keyCode >= VKEY_A && keyCode <= VKEY_Z;
	const bool isNumber = keyCode >= VKEY_0 && keyCode <= VKEY_9;

	if (isLetter || isNumber)
	{
		retChar = keyCode;
	}
	else
	{
		// First convert any special keys to the corresponding ascii char
		switch (keyCode)
		{
		case VKEY_PERIOD: retChar = '.'; break;
		case VKEY_COMMA: retChar = ','; break;
		case VKEY_PLUS: retChar = '='; break;
		case VKEY_MINUS: retChar = '-'; break;
		case VKEY_COLON: retChar = ';'; break;
		case VKEY_QUESTION: retChar = '/'; break;
		case VKEY_SQUIGGLE: retChar = '`'; break;
		case VKEY_LEFT_BRACKET: retChar = '['; break;
		case VKEY_BACK_SLASH: retChar = '\\'; break;
		case VKEY_RIGHT_BRACKET: retChar = ']'; break;
		case VKEY_QUOTE: retChar = '\''; break;
		case VKEY_SPACE: retChar = ' '; break;
		}
	}

	if (retChar >= 'A' &&
		retChar <= 'Z' &&
		!shiftDown)
	{
		// If not shifted, make the character lower-case.
		// TODO: handle caps lock state.
		retChar += 32;
	}
	else if (shiftDown)
	{
		switch (retChar)
		{
		case '`': retChar = '~'; break;
		case '1': retChar = '!'; break;
		case '2': retChar = '@'; break;
		case '3': retChar = '#'; break;
		case '4': retChar = '$'; break;
		case '5': retChar = '%'; break;
		case '6': retChar = '^'; break;
		case '7': retChar = '&'; break;
		case '8': retChar = '*'; break;
		case '9': retChar = '('; break;
		case '0': retChar = ')'; break;
		case '-': retChar = '_'; break;
		case '=': retChar = '+'; break;
		case '[': retChar = '{'; break;
		case ']': retChar = '}'; break;
		case '\\': retChar = '|'; break;
		case ';': retChar = ':'; break;
		case '\'': retChar = '\"'; break;
		case ',': retChar = '<'; break;
		case '.': retChar = '>'; break;
		case '/': retChar = '?'; break;
		}
	}

	return retChar;
}
