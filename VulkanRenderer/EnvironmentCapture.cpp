#include "EnvironmentCapture.h"
#include "Constants.h"
#include "Renderer.h"

VkRenderPass EnvironmentCapture::sRenderPass = VK_NULL_HANDLE;

EnvironmentCapture::EnvironmentCapture() :
	mImage(VK_NULL_HANDLE),
	mImageMemory(VK_NULL_HANDLE),
	mCubemapImageView(VK_NULL_HANDLE),
	mSampler(VK_NULL_HANDLE),
	mDepthImage(VK_NULL_HANDLE),
	mDepthImageMemory(VK_NULL_HANDLE),
	mDepthImageView(VK_NULL_HANDLE),
	mResolution(DEFAULT_ENVIRONMENT_CAPTURE_RESOLUTION),
	mCapturedResolution(0)
{
	for (VkImageView& view : mFaceImageViews)
	{
		view = VK_NULL_HANDLE;
	}

	for (VkFramebuffer& framebuffer : mFramebuffers)
	{
		framebuffer = VK_NULL_HANDLE;
	}
}

EnvironmentCapture::~EnvironmentCapture()
{
	DestroyCubemap();
	DestroyFramebuffers();
}

void EnvironmentCapture::Capture()
{
	CreateRenderPass();

	if (mCapturedResolution != mResolution)
	{
		// Destroy old texture if it exists
		DestroyCubemap();
		DestroyFramebuffers();

		// Recreate cubemap at proper resolution
		CreateCubemap();
		CreateFramebuffers();
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
		assert(mCubemapImageView != VK_NULL_HANDLE);
		assert(mImageMemory != VK_NULL_HANDLE);
		assert(mSampler != VK_NULL_HANDLE);
		assert(mDepthImage != VK_NULL_HANDLE);
		assert(mDepthImageMemory != VK_NULL_HANDLE);
		assert(mDepthImageView != VK_NULL_HANDLE);

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

		vkDestroyImage(device, mDepthImage, nullptr);
		mDepthImage = VK_NULL_HANDLE;

		vkFreeMemory(device, mDepthImageMemory, nullptr);
		mDepthImageMemory = VK_NULL_HANDLE;

		vkDestroyImageView(device, mDepthImageView, nullptr);
		mDepthImageView = VK_NULL_HANDLE;
	}
}

void EnvironmentCapture::CreateCubemap()
{
	assert(mImage == VK_NULL_HANDLE);
	assert(mImageMemory == VK_NULL_HANDLE);
	assert(mSampler == VK_NULL_HANDLE);
	assert(mCubemapImageView == VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

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

	CreateDepthImage();

	CreateImageViews();
}

void EnvironmentCapture::CreateDepthImage()
{
	assert(mDepthImage == VK_NULL_HANDLE);
	assert(mDepthImageMemory == VK_NULL_HANDLE);
	assert(mDepthImageView == VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	Texture::CreateImage(mResolution,
		mResolution,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mDepthImage,
		mDepthImageMemory);

	mDepthImageView = Texture::CreateImageView(mDepthImage,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_ASPECT_DEPTH_BIT);

	Texture::TransitionImageLayout(mDepthImage,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	vkDeviceWaitIdle(device);
}

void EnvironmentCapture::DestroyFramebuffers()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	for (VkFramebuffer& framebuffer : mFramebuffers)
	{
		if (framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
			framebuffer = VK_NULL_HANDLE;
		}
	}
}

void EnvironmentCapture::CreateFramebuffers()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	VkRenderPass renderPass = renderer->GetRenderPass();

	GBuffer& gBuffer = renderer->GetGBuffer();

	for (uint32_t i = 0; i < 6; ++i)
	{
		std::vector<VkImageView> attachments;
		attachments.push_back(mFaceImageViews[i]);
		attachments.push_back(mDepthImage);

		for (uint32_t g = 0; g < gBuffer.GetImageViews().size(); ++g)
		{
			attachments.push_back(gBuffer.GetImageViews()[g]);
		}

		VkFramebufferCreateInfo ciFramebuffer = {};
		ciFramebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		ciFramebuffer.renderPass = renderer->GetRenderPass();
		ciFramebuffer.attachmentCount = attachments.size();
		ciFramebuffer.pAttachments = attachments.data();
		ciFramebuffer.width = mResolution;
		ciFramebuffer.height = mResolution;
		ciFramebuffer.layers = 1;

		if (vkCreateFramebuffer(device, &ciFramebuffer, nullptr, &mFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create framebuffer.");
		}
	}
}

void EnvironmentCapture::CreateImageViews()
{
	// Create the cubemap image view first
	VkDevice device = Renderer::Get()->GetDevice();

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = mImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
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
		ciImageView.format = VK_FORMAT_R8G8B8A8_UNORM;
		ciImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ciImageView.subresourceRange.baseMipLevel = 0;
		ciImageView.subresourceRange.levelCount = 1;
		ciImageView.subresourceRange.baseArrayLayer = i;
		ciImageView.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &viewInfo, nullptr, &mFaceImageViews[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create texture image view");
		}
	}
}

void EnvironmentCapture::CreateRenderPass()
{
	//if (sRenderPass != VK_NULL_HANDLE)
	//{
	//	return;
	//}

	
}