#include "TextureCube.h"
#include "Renderer.h"

TextureCube::TextureCube()
{
	mTextureType = TextureType::TextureCube;
	mLayers = 6;
	mMipLevels = 1;

	for (int32_t i = 0; i < 6; ++i)
	{
		mFaceImageViews[i] = VK_NULL_HANDLE;
	}
}

void TextureCube::Load(const std::string& path)
{
	// TODO! Load an HDR cubemap
	throw std::exception("Cubemap load not implemented yet");
}

void TextureCube::Create(uint32_t width, uint32_t height, VkFormat format)
{
	mWidth = width;
	mHeight = height;
	mFormat = format;

	CreateImage(mWidth,
				mHeight,
				mFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				mImage,
				mImageMemory,
				mMipLevels,
				mLayers,
				VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
	
	// TODO: Should we generate mips here? Or after transition to SRV? Called from owning class?
	//GenerateMips();

	CreateTextureSampler();
	mImageView = CreateImageView(mImage, mFormat, VK_IMAGE_ASPECT_COLOR_BIT, mMipLevels, mLayers, TextureType::TextureCube);
	CreateFaceImageViews();
	Clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
}

VkImageView TextureCube::GetFaceImageView(uint32_t index)
{
	assert(index >= 0);
	assert(index < 6);
	return mFaceImageViews[index];
}

void TextureCube::Destroy()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mImage != VK_NULL_HANDLE)
	{
		for (VkImageView& view : mFaceImageViews)
		{
			assert(view != VK_NULL_HANDLE);
			vkDestroyImageView(device, view, nullptr);
			view = VK_NULL_HANDLE;
		}
	}

	Texture::Destroy();
}

void TextureCube::CreateFaceImageViews()
{
	// Create the cubemap image view first
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	// Now create individual image views 
	for (uint32_t i = 0; i < 6; ++i)
	{
		VkImageViewCreateInfo ciImageView = {};
		ciImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ciImageView.image = mImage;
		ciImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ciImageView.format = mFormat;
		ciImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ciImageView.subresourceRange.baseMipLevel = 0;
		ciImageView.subresourceRange.levelCount = mMipLevels;
		ciImageView.subresourceRange.baseArrayLayer = i;
		ciImageView.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &ciImageView, nullptr, &mFaceImageViews[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create texture image view");
		}
	}
}
