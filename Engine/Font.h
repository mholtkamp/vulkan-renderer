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
	int32_t mAdvance;
};

struct Font
{
	Font(std::string name,
		int32_t size,
		bool bold,
		bool italic,
		int32_t width,
		int32_t height,
		int32_t characterCount,
		Character* characters,
		std::string texturePath,
		bool distanceField)
	{
		mName = name;
		mSize = size;
		mBold = bold;
		mItalic = italic;
		mWidth = width;
		mHeight = height;
		mCharacterCount = characterCount;
		mCharacters = characters;
		mTexturePath = texturePath;
		mTexture = nullptr;
		mDistanceField = distanceField;
	}

	std::string mName;
	int32_t mSize;
	bool mBold;
	bool mItalic;
	int32_t mWidth;
	int32_t mHeight;
	int32_t mCharacterCount;
	Character* mCharacters;
	std::string mTexturePath;
	class Texture2D* mTexture;
	bool mDistanceField;

	void Create();
	void Destroy();
};
