#include "Renderer.h"
#include "Texture2D.h"

#include <stb_image.h>
#include <exception>

Texture2D::Texture2D()
{
	mTextureType = TextureType::Texture2D;
	mLayers = 1;
	mMipLevels = 1;
}

void Texture2D::Create(uint32_t width, uint32_t height, VkFormat format)
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
		mLayers);

	//GenerateMips();

	CreateTextureSampler();
	mImageView = CreateImageView(mImage, mFormat, VK_IMAGE_ASPECT_COLOR_BIT, mMipLevels, mLayers);
	Clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
}

void Texture2D::Load(const std::string& path)
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	VkPhysicalDevice physicalDevice = renderer->GetPhysicalDevice();

	// Destroy if image already exists.
	Destroy();

	int32_t texWidth;
	int32_t texHeight;
	int32_t texChannels;
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

	mName = path;

	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	int32_t imageSizeInt = static_cast<int32_t>(imageSize);

	if (pixels == nullptr)
	{
		throw std::exception("Failed to load texture image");
	}

	mWidth = texWidth;
	mHeight = texHeight;
	mMipLevels = static_cast<int32_t>(floor(log2(std::max(mWidth, mHeight))) + 1);

	VkBuffer stagingBuffer;
	Allocation stagingBufferMemory;

	renderer->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory.mDeviceMemory, stagingBufferMemory.mOffset, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory.mDeviceMemory);

	stbi_image_free(pixels);

	mFormat = VK_FORMAT_R8G8B8A8_UNORM;
	CreateImage(texWidth, texHeight, mFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mImage, mImageMemory, mMipLevels, mLayers);

	TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mMipLevels);
	CopyBufferToImage(stagingBuffer, mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	Allocator::Free(stagingBufferMemory);

	GenerateMips();
	CreateTextureSampler();
	mImageView = CreateImageView(mImage, mFormat, VK_IMAGE_ASPECT_COLOR_BIT, mMipLevels, mLayers);
}