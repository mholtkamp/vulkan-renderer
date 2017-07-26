#include "Cubemap.h"
#include "Renderer.h"

Cubemap::Cubemap() : 
	mImage(VK_NULL_HANDLE),
	mImageMemory(VK_NULL_HANDLE),
	mCubemapImageView(VK_NULL_HANDLE),
	mSampler(VK_NULL_HANDLE),
	mResolution(0),
	mFormat(VK_FORMAT_R8G8B8A8_UNORM)
{
	for (int32_t i = 0; i < 6; ++i)
	{
		mFaceImageViews[i] = VK_NULL_HANDLE;
	}
}

void Cubemap::Create(uint32_t resolution, VkFormat format)
{
	mResolution = resolution;
	mFormat = format;

	CreateImage();
	CreateSampler();
	CreateImageViews();
}

Cubemap::~Cubemap()
{
	Destroy();
}

bool Cubemap::IsValid()
{
	return mImage != VK_NULL_HANDLE;
}

VkImageView Cubemap::GetCubemapImageView()
{
	return mCubemapImageView;
}

VkSampler Cubemap::GetSampler()
{
	return mSampler;
}

VkImageView Cubemap::GetFaceImageView(uint32_t index)
{
	assert(index >= 0);
	assert(index < 6);
	return mFaceImageViews[index];
}

void Cubemap::Destroy()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mImage != VK_NULL_HANDLE)
	{
		assert(mCubemapImageView != VK_NULL_HANDLE);
		assert(mSampler != VK_NULL_HANDLE);
		assert(mImageMemory != VK_NULL_HANDLE);

		vkDestroyImage(device, mImage, nullptr);
		mImage = VK_NULL_HANDLE;

		vkDestroyImageView(device, mCubemapImageView, nullptr);
		mCubemapImageView = VK_NULL_HANDLE;

		vkDestroySampler(device, mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;

		vkFreeMemory(device, mImageMemory, nullptr);
		mImageMemory = VK_NULL_HANDLE;

		for (VkImageView& view : mFaceImageViews)
		{
			assert(view != VK_NULL_HANDLE);
			vkDestroyImageView(device, view, nullptr);
			view = VK_NULL_HANDLE;
		}
	}
}

void Cubemap::CreateImage()
{
	assert(mImage == VK_NULL_HANDLE);
	assert(mImageMemory == VK_NULL_HANDLE);
	assert(mSampler == VK_NULL_HANDLE);
	assert(mCubemapImageView == VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	// Create optimal tiled target image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = mFormat;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent = { mResolution, mResolution, 1 };
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageCreateInfo.arrayLayers = 6;
	imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &mImage) != VK_SUCCESS)
	{
		throw std::exception("Failed to create cubemap image in evironment capture");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(device, mImage, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = renderer->FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS)
	{
		throw std::exception("Failed to allocate image memory for cubemap");
	}

	if (vkBindImageMemory(device, mImage, mImageMemory, 0) != VK_SUCCESS)
	{
		throw std::exception("Failed to bind memory to image for cubemap");
	}

	//Texture::TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
}

void Cubemap::CreateImageViews()
{
	// Create the cubemap image view first
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = mImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = mFormat;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	if (vkCreateImageView(device, &viewInfo, nullptr, &mCubemapImageView) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture image view");
	}

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
		ciImageView.subresourceRange.levelCount = 1;
		ciImageView.subresourceRange.baseArrayLayer = i;
		ciImageView.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &ciImageView, nullptr, &mFaceImageViews[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create texture image view");
		}
	}
}

void Cubemap::CreateSampler()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkSamplerCreateInfo ciSampler = {};
	ciSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	ciSampler.magFilter = VK_FILTER_LINEAR;
	ciSampler.minFilter = VK_FILTER_LINEAR;
	ciSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	ciSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	ciSampler.addressModeV = ciSampler.addressModeU;
	ciSampler.addressModeW = ciSampler.addressModeU;
	ciSampler.mipLodBias = 0.0f;
	ciSampler.compareOp = VK_COMPARE_OP_ALWAYS;
	ciSampler.compareEnable = VK_FALSE;
	ciSampler.minLod = 0.0f;
	ciSampler.maxLod = 0.0f;
	ciSampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	ciSampler.maxAnisotropy = 1.0f;
	ciSampler.anisotropyEnable = VK_FALSE;

	if (vkCreateSampler(device, &ciSampler, nullptr, &mSampler) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture sampler");
	}
}
