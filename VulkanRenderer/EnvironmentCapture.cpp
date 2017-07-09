#include "EnvironmentCapture.h"
#include "Constants.h"
#include "Renderer.h"

VkRenderPass EnvironmentCapture::sRenderPass = VK_NULL_HANDLE;

EnvironmentCapture::EnvironmentCapture() :
	mFramebuffer(VK_NULL_HANDLE),
	mImage(VK_NULL_HANDLE),
	mImageMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),
	mSampler(VK_NULL_HANDLE),
	mResolution(DEFAULT_ENVIRONMENT_CAPTURE_RESOLUTION),
	mCapturedResolution(0)
{

}

EnvironmentCapture::~EnvironmentCapture()
{
	DestroyCubemap();
}

void EnvironmentCapture::Capture()
{
	CreateRenderPass();

	if (mCapturedResolution != mResolution)
	{
		// Destroy old texture if it exists
		DestroyCubemap();
		DestroyFramebuffer();

		// Recreate cubemap at proper resolution
		CreateCubemap();
		CreateFramebuffer();
	}

	mCapturedResolution = mResolution;
}

void EnvironmentCapture::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void EnvironmentCapture::SetTextureResolution(uint32_t size)
{
	if (size == 0 ||
		size > ENVIRONMENT_CAPTURE_MAX_RESOLUTION)
	{
		throw std::exception("Environment Capture invalid resolution");
	}

	mResolution = size;
}


void EnvironmentCapture::DestroyCubemap()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mImage != VK_NULL_HANDLE)
	{
		assert(mImageView != VK_NULL_HANDLE);
		assert(mImageMemory != VK_NULL_HANDLE);
		assert(mSampler != VK_NULL_HANDLE);

		vkDestroyImage(device, mImage, nullptr);
		mImage = VK_NULL_HANDLE;

		vkDestroyImageView(device, mImageView, nullptr);
		mImageView = VK_NULL_HANDLE;

		vkDestroySampler(device, mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;

		vkFreeMemory(device, mImageMemory, nullptr);
		mImageMemory = VK_NULL_HANDLE;
	}
}

void EnvironmentCapture::CreateCubemap()
{
	assert(mImage == VK_NULL_HANDLE);
	assert(mImageMemory == VK_NULL_HANDLE);
	assert(mSampler == VK_NULL_HANDLE);
	assert(mImageMemory == VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	
	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//VkDeviceSize imageSize = mResolution * mResolution * 4 * 6; // 4 channels, 6 faces of the cube

	//renderer->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//void* data;
	//vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	//memset(data, 128, static_cast<size_t>(imageSize));
	//vkUnmapMemory(device, stagingBufferMemory);

	// Create optimal tiled target image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent = { mResolution, mResolution, 1 };
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.arrayLayers = 6;
	imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &mImage) != VK_SUCCESS)
	{
		throw std::exception("Failed to create cubemap image in evironment capture");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(device, mImage, &memReqs);

	VkMemoryAllocateInfo allocInfo;
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

	Texture::TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	mImageView = Texture::CreateImageView(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

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

void EnvironmentCapture::DestroyFramebuffer()
{

}

void EnvironmentCapture::CreateFramebuffer()
{

}