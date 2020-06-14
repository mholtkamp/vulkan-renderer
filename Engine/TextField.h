#pragma once

#include "Button.h"
#include "Quad.h"

typedef void(*TextFieldHandlerFP)(class TextField* textField);

class TextField : public Button
{
public:

	TextField();
	~TextField();

	virtual void Update() override;
	virtual void SetState(ButtonState newState) override;

	void SetTextEditHandler(TextFieldHandlerFP handler);
	void SetTextConfirmHandler(TextFieldHandlerFP handler);

protected:

	static void SetSelectedTextField(TextField* newField);

	static TextField* sSelectedTextField;
	static float sCursorBlinkTime;
	static const float sCursorBlinkPeriod;

	TextFieldHandlerFP mTextEditHandler;
	TextFieldHandlerFP mTextConfirmHandler;

	Quad* mCursorQuad;
	int32_t mMaxCharacters;
};