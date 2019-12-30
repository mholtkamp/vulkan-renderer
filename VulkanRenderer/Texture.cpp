#include "Texture.h"
#include "Renderer.h"

#include <stb_image.h>
#include <exception>

using namespace std;

Texture::Texture() :
	mImage(VK_NULL_HANDLE),
	mImageMemory(VK_NULL_HANDLE),
	mImageView(VK_NULL_HANDLE),
	mSampler(VK_NULL_HANDLE),
	mTextureType(TextureType::Texture2D),
	mWidth(0),
	mHeight(0),
	mMipLevels(1),
	mLayers(1)
{

}

Texture::~Texture()
{
	Destroy();
}

void Texture::Destroy()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer ? Renderer::Get()->GetDevice() : nullptr;

	if (mImage != VK_NULL_HANDLE)
	{
		vkDestroySampler(device, mSampler, nullptr);
		vkDestroyImageView(device, mImageView, nullptr);
		vkDestroyImage(device, mImage, nullptr);
		vkFreeMemory(device, mImageMemory, nullptr);

		mImage = VK_NULL_HANDLE;
		mImageView = VK_NULL_HANDLE;
		mImageMemory = VK_NULL_HANDLE;
		mSampler = VK_NULL_HANDLE;

		mName.clear();
	}
}

void Texture::Load(const std::string& path)
{
	
}

void Texture::GenerateMips()
{
	Renderer* renderer = Renderer::Get();
	VkCommandBuffer blitCmd = renderer->BeginSingleSubmissionCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = mImage;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	// Copy down mips from n-1 to n
	for (uint32_t f = 0; f < mLayers; ++f)
	{
		for (uint32_t i = 1; i < mMipLevels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(blitCmd,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit imageBlit{};

			int32_t srcWidth = int32_t(mWidth >> (i - 1));
			int32_t srcHeight = int32_t(mHeight >> (i - 1));
			int32_t dstWidth = int32_t(mWidth >> i);
			int32_t dstHeight = int32_t(mHeight >> i);

			// Source
			imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.srcSubresource.layerCount = 1;
			imageBlit.srcSubresource.mipLevel = i - 1;
			imageBlit.srcSubresource.baseArrayLayer = f;
			imageBlit.srcOffsets[1].x = srcWidth > 0 ? srcWidth : 1;
			imageBlit.srcOffsets[1].y = srcHeight > 0 ? srcHeight : 1;
			imageBlit.srcOffsets[1].z = 1;

			// Destination
			imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBlit.dstSubresource.layerCount = 1;
			imageBlit.dstSubresource.mipLevel = i;
			imageBlit.dstSubresource.baseArrayLayer = f;
			imageBlit.dstOffsets[1].x = dstWidth > 0 ? dstWidth : 1;
			imageBlit.dstOffsets[1].y = dstHeight > 0 ? dstHeight : 1;
			imageBlit.dstOffsets[1].z = 1;

			// Blit from previous level
			vkCmdBlitImage(
				blitCmd,
				mImage,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				mImage,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&imageBlit,
				VK_FILTER_LINEAR);

			// Barrier to transition the mip we copied from to SRV layout
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(blitCmd,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}

		// Transition the last mips layout to Shader Read Only
		barrier.subresourceRange.baseMipLevel = mMipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(blitCmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	renderer->EndSingleSubmissionCommands(blitCmd);
}

bool Texture::IsValid() const
{
	return mImage != VK_NULL_HANDLE;
}

const std::string& Texture::GetName() const
{
	return mName;
}

VkImageView Texture::GetImageView()
{
	return mImageView;
}

VkSampler Texture::GetSampler()
{
	return mSampler;
}

void Texture::CreateImage(uint32_t width,
	uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory,
	uint32_t mipLevels,
	uint32_t layers,
	VkImageCreateFlags flags)
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkImageCreateInfo ciImage = {};
	ciImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ciImage.imageType = VK_IMAGE_TYPE_2D;
	ciImage.extent.width = static_cast<uint32_t>(width);
	ciImage.extent.height = static_cast<uint32_t>(height);
	ciImage.extent.depth = 1;
	ciImage.mipLevels = mipLevels;
	ciImage.arrayLayers = layers;
	ciImage.format = format;
	ciImage.tiling = tiling;
	ciImage.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	ciImage.usage = usage;
	ciImage.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ciImage.samples = VK_SAMPLE_COUNT_1_BIT;
	ciImage.flags = flags;

	if (vkCreateImage(device, &ciImage, nullptr, &image) != VK_SUCCESS)
	{
		throw exception("Failed to create image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = renderer->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw exception("Failed to allocate image memory");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView Texture::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t layers, TextureType type)
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkImageViewCreateInfo view = {};
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.image = image;
	view.viewType = (type == TextureType::Texture2D) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_CUBE;
	view.format = format;
	view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	view.subresourceRange.aspectMask = aspectFlags;
	view.subresourceRange.baseMipLevel = 0;
	view.subresourceRange.baseArrayLayer = 0;
	view.subresourceRange.layerCount = layers;
	view.subresourceRange.levelCount = mipLevels;

	VkImageView imageView;
	if (vkCreateImageView(device, &view, nullptr, &imageView) != VK_SUCCESS)
	{
		throw exception("Failed to create texture image view");
	}

	return imageView;
}

void Texture::CreateTextureSampler()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkSamplerCreateInfo sampler = {};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler.mipLodBias = 0.0f;
	sampler.compareEnable = VK_FALSE;
	sampler.compareOp = VK_COMPARE_OP_NEVER;
	sampler.minLod = 0.0f;
	sampler.maxLod = 0.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler.maxAnisotropy = 1.0;
	sampler.anisotropyEnable = VK_FALSE;

	// With mip mapping
	sampler.maxLod = (float)mMipLevels;
	vkCreateSampler(device, &sampler, nullptr, &mSampler);

	if (vkCreateSampler(device, &sampler, nullptr, &mSampler) != VK_SUCCESS)
	{
		throw exception("Failed to create texture sampler");
	}
}

void Texture::TransitionLayout(VkImageLayout newLayout)
{
	if (mImage != VK_NULL_HANDLE)
	{
		Texture::TransitionImageLayout(mImage, mFormat, mLayout, newLayout, mMipLevels, mLayers);
		mLayout = newLayout;
	}
}

void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int32_t mipLevels, int32_t layerCount, VkCommandBuffer commandBuffer)
{
	if (newLayout == oldLayout)
		return;

	Renderer* renderer = Renderer::Get();

	VkCommandBuffer singleCb = VK_NULL_HANDLE;

	if (commandBuffer == VK_NULL_HANDLE)
	{
		singleCb = renderer->BeginSingleSubmissionCommands();
		commandBuffer = singleCb;
	}

	VkPipelineStageFlags srcMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags dstMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;

	if (format == VK_FORMAT_D16_UNORM)
	{
		// Shadow maps are a depth-only format. Do not use stencil bit.
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else
	{
		throw std::invalid_argument("Unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer,
		srcMask,
		dstMask,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	if (singleCb != VK_NULL_HANDLE)
	{
		renderer->EndSingleSubmissionCommands(commandBuffer);
	}
}

void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	Renderer* renderer = Renderer::Get();
	VkCommandBuffer commandBuffer = renderer->BeginSingleSubmissionCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region);


	renderer->EndSingleSubmissionCommands(commandBuffer);
}

void Texture::Clear(glm::vec4 color)
{
	if (mImage != VK_NULL_HANDLE)
	{
		VkImageLayout originalLayout = mLayout;
		TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		Renderer* renderer = Renderer::Get();
		VkCommandBuffer cb = renderer->BeginSingleSubmissionCommands();

		VkClearColorValue clearValue;
		memset(&clearValue, 0, sizeof(VkClearColorValue));
		clearValue.float32[0] = color.r;
		clearValue.float32[1] = color.g;
		clearValue.float32[2] = color.b;
		clearValue.float32[3] = color.a;

		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.layerCount = mLayers;
		subresourceRange.levelCount = mMipLevels;

		vkCmdClearColorImage(cb, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearValue, 1, &subresourceRange);
		renderer->EndSingleSubmissionCommands(cb);

		TransitionLayout(originalLayout != VK_IMAGE_LAYOUT_UNDEFINED ? originalLayout : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

void Texture::SetLayout(VkImageLayout newLayout)
{
	// Render passes can inherantly change image layouts.
	// This function is meant to be used to rectify these under-the-hood transitions.
	mLayout = newLayout;
}

void Texture::TransitionToRT()
{
	TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

void Texture::TransitionToSRV()
{
	TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
