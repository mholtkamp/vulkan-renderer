#pragma once

#include "Button.h"

class TextField : public Button
{
public:

	TextField();
	~TextField();

	virtual void Update() override;

protected:

	static TextField* sSelectedTextField;

	Quad* mCursorQuad;
	int32_t mMaxCharacters;
};