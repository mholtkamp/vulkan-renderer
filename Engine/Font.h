#pragma once

#include <stdint.h>
#include <string>

struct Character
{
	int32_t mCodePoint;
	int32_t mX;
	int32_t mY;
	int32_t mWidth;
	int32_t mHeight;
	int32_t mOriginX;
	int32_t mOriginY;
};

struct Font
{
	Font() :
		mName("Font"),
		mSize(0),
		mBold(false),
		mItalic(false),
		mWidth(0),
		mHeight(0),
		mCharacterCount(0),
		mCharacters(nullptr),
		mTexture(nullptr)
	{

	}

	std::string mName;
	int32_t mSize;
	bool mBold;
	bool mItalic;
	int32_t mWidth;
	int32_t mHeight;
	int32_t mCharacterCount;
	Character* mCharacters;
	class Texture2D* mTexture;
};
