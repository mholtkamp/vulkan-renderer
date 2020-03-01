#include "GBuffer.h"
#include "Renderer.h"
#include "Allocator.h"

#include <vulkan/vulkan.h>
#include <exception>

using namespace std;

GBuffer::GBuffer() :
    mWidth(0),
    mHeight(0),
    mSampler(VK_NULL_HANDLE)
{

}

GBuffer::~GBuffer()
{

}

void GBuffer::Create(uint32_t width, uint32_t height)
{
    mWidth = width;
    mHeight = height;
	CreateImages();
    CreateSampler();
}

void GBuffer::Destroy()
{
    if (mImages.size() == 0)
    {
        return;
    }

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	for (size_t i = 0; i < mImages.size(); ++i)
	{
		vkDestroyImage(device, mImages[i], nullptr);
		vkDestroyImageView(device, mImageViews[i], nullptr);
		Allocator::Free(mImageMemory[i]);
	}

    mImages.clear();
    mImageViews.clear();
    mImageMemory.clear();
    mFormats.clear();

    if (mSampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(device, mSampler, nullptr);
        mSampler = VK_NULL_HANDLE;
    }
}

std::vector<VkImage>& GBuffer::GetImage()
{
	return mImages;
}

std::vector<VkImageView>& GBuffer::GetImageViews()
{
	return mImageViews;
}

std::vector<VkFormat>& GBuffer::GetFormats()
{
	return mFormats;
}

void GBuffer::CreateImages()
{
	assert(mImages.size() == 0);

	mImages.resize(GB_COUNT);
	mImageMemory.resize(GB_COUNT);
	mImageViews.resize(GB_COUNT);
	mFormats.resize(GB_COUNT);

	CreateAttachment(GB_POSITION, VK_FORMAT_R16G16B16A16_SFLOAT);
	CreateAttachment(GB_NORMAL, VK_FORMAT_R16G16B16A16_SFLOAT);
	CreateAttachment(GB_COLOR, VK_FORMAT_R8G8B8A8_UNORM);
	CreateAttachment(GB_SPECULAR, VK_FORMAT_R8G8B8A8_UNORM);
    CreateAttachment(GB_METALLIC, VK_FORMAT_R8_UNORM);
    CreateAttachment(GB_ROUGHNESS, VK_FORMAT_R8_UNORM);
}

void GBuffer::CreateSampler()
{
    if (mSampler != VK_NULL_HANDLE)
    {
        return;
    }

	// Create sampler to sample from the color attachments
	VkSamplerCreateInfo ciSampler = {};
	ciSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	ciSampler.magFilter = VK_FILTER_NEAREST;
	ciSampler.minFilter = VK_FILTER_NEAREST;
	ciSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	ciSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	ciSampler.addressModeV = ciSampler.addressModeU;
	ciSampler.addressModeW = ciSampler.addressModeU;
	ciSampler.mipLodBias = 0.0f;
	ciSampler.maxAnisotropy = 1.0f;
	ciSampler.minLod = 0.0f;
	ciSampler.maxLod = 1.0f;
	ciSampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

	if (vkCreateSampler(Renderer::Get()->GetDevice(), &ciSampler, nullptr, &mSampler) != VK_SUCCESS)
	{
		throw exception("Failed to create GBuffer sampler");
	}
}

void GBuffer::CreateAttachment(GBufferIndex index, VkFormat format)
{
	Renderer* renderer = Renderer::Get();

	VkImage& image = mImages[index];
	Allocation& imageMemory = mImageMemory[index];
	VkImageView& imageView = mImageViews[index];

	// Save the format in case it is needed later.
	mFormats[index] = format;

	VkImageUsageFlags usage;
	VkImageAspectFlags aspect;
	VkImageLayout layout;

	usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	aspect = VK_IMAGE_ASPECT_COLOR_BIT;
	layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	Texture::CreateImage(mWidth,
		mHeight,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		image,
		imageMemory);

	imageView = Texture::CreateImageView(image,
		format,
		aspect);

	Texture::TransitionImageLayout(image,
		format,
		VK_IMAGE_LAYOUT_UNDEFINED,
		layout);
}

VkSampler GBuffer::GetSampler()
{
	return mSampler;
}