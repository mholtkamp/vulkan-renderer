#include "Font.h"
#include "Texture2D.h"

void Font::Create()
{
	mTexture = new Texture2D();
	mTexture->Load(mTexturePath);
}

void Font::Destroy()
{
	if (mTexture != nullptr)
	{
		mTexture->Destroy();
		mTexture = nullptr;
	}
}