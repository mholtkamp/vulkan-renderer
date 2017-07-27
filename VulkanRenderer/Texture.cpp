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
	mWidth(0),
	mHeight(0),
	mMipLevels(0)
{

}

Texture::~Texture()
{
	Destroy();
}

void Texture::Destroy()
{
	VkDevice device = Renderer::Get()->GetDevice();

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
	}
}

void Texture::Load(const std::string& path)
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	VkPhysicalDevice physicalDevice = renderer->GetPhysicalDevice();

	int texWidth;
	int texHeight;
	int texChannels;
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

	mName = path;

	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	int32_t imageSizeInt = static_cast<int32_t>(imageSize);

	if (pixels == nullptr)
	{
		throw exception("Failed to load texture image");
	}

	mWidth = texWidth;
	mHeight = texHeight;

	VkFormatProperties formatProperties;

	// calculate num of mip maps
	// numLevels = 1 + floor(log2(max(w, h, d)))
	// Calculated as log2(max(width, height, depth))c + 1 (see specs)
	mMipLevels = static_cast<int32_t>(floor(log2(std::max(mWidth, mHeight))) + 1);

	// Get device properites for the requested texture format
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	// Mip-chain generation requires support for blit source and destination
	assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
	assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

	VkMemoryAllocateInfo memAllocInfo = {};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	// !!!!!!!!!!!! CONTINUE HERE !!!!!!!!!!!!!!!
	VkMemoryRequirements memReqs = {};

	// Create a host-visible staging buffer that contains the raw image data
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = imageSize;

	// This buffer is used as a transfer source for the buffer copy
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
	{
		throw std::exception("Failed to create staging buffer");
	}

	vkGetBufferMemoryRequirements(device, stagingBuffer, &memReqs);
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = renderer->FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device, &memAllocInfo, nullptr, &stagingMemory) != VK_SUCCESS) { throw std::exception("Failed to allocate staging buffer memory."); }
	if (vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0) != VK_SUCCESS) { throw std::exception("Failed to allocate bind staging buffer to memory."); }

	// Copy texture data into staging buffer
	void* data;
	vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, &data);
	memcpy(data, pixels, imageSizeInt);
	vkUnmapMemory(device, stagingMemory);

	// Create optimal tiled target image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.mipLevels = mMipLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent = { mWidth, mHeight, 1 };
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	if (vkCreateImage(device, &imageCreateInfo, nullptr, &mImage) != VK_SUCCESS)
	{
		throw exception("Failed to create image");
	}

	vkGetImageMemoryRequirements(device, mImage, &memReqs);
	memAllocInfo.allocationSize = memReqs.size;
	memAllocInfo.memoryTypeIndex = renderer->FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(device, &memAllocInfo, nullptr, &mImageMemory) != VK_SUCCESS) { throw exception("Failed to allocate memory."); }
	if (vkBindImageMemory(device, mImage, mImageMemory, 0) != VK_SUCCESS) { throw exception("Failed to bind image to memory."); }

	VkCommandBuffer copyCmd = renderer->BeginSingleSubmissionCommands();

	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	// Copy the first mip of the chain, remaining mips will be generated
	VkBufferImageCopy bufferCopyRegion = {};
	bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferCopyRegion.imageSubresource.mipLevel = 0;
	bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = mWidth;
	bufferCopyRegion.imageExtent.height = mHeight;
	bufferCopyRegion.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(copyCmd, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

	TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(stagingBuffer, mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	renderer->EndSingleSubmissionCommands(copyCmd);

	// Clean up staging resources
	vkFreeMemory(device, stagingMemory, nullptr);
	vkDestroyBuffer(device, stagingBuffer, nullptr);

	// Generate the mip chain
	// ---------------------------------------------------------------
	// We copy down the whole mip chain doing a blit from mip-1 to mip
	// An alternative way would be to always blit from the first mip level and sample that one down
	VkCommandBuffer blitCmd = renderer->BeginSingleSubmissionCommands();

	// Copy down mips from n-1 to n
	for (uint32_t i = 1; i < mMipLevels; i++)
	{
		VkImageBlit imageBlit{};

		// Source
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.layerCount = 1;
		imageBlit.srcSubresource.mipLevel = i - 1;
		imageBlit.srcOffsets[1].x = int32_t(mWidth >> (i - 1));
		imageBlit.srcOffsets[1].y = int32_t(mHeight >> (i - 1));
		imageBlit.srcOffsets[1].z = 1;

		// Destination
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.layerCount = 1;
		imageBlit.dstSubresource.mipLevel = i;
		imageBlit.dstOffsets[1].x = int32_t(mWidth >> i);
		imageBlit.dstOffsets[1].y = int32_t(mHeight >> i);
		imageBlit.dstOffsets[1].z = 1;

		VkImageSubresourceRange mipSubRange = {};
		mipSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		mipSubRange.baseMipLevel = i;
		mipSubRange.levelCount = 1;
		mipSubRange.layerCount = 1;

		// Transiton current mip level to transfer dest
		//vks::tools::setImageLayout(
		//	blitCmd,
		//	texture.image,
		//	VK_IMAGE_LAYOUT_UNDEFINED,
		//	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		//	mipSubRange,
		//	VK_PIPELINE_STAGE_TRANSFER_BIT,
		//	VK_PIPELINE_STAGE_HOST_BIT);

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

		// Transiton current mip level to transfer source for read in next iteration
		//vks::tools::setImageLayout(
		//	blitCmd,
		//	texture.image,
		//	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		//	VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		//	mipSubRange,
		//	VK_PIPELINE_STAGE_HOST_BIT,
		//	VK_PIPELINE_STAGE_TRANSFER_BIT);
	}

	// After the loop, all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ
	subresourceRange.levelCount = mMipLevels;
	//vks::tools::setImageLayout(
	//	blitCmd,
	//	texture.image,
	//	VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	//	texture.imageLayout,
	//	subresourceRange);

	renderer->EndSingleSubmissionCommands(blitCmd);
	// ---------------------------------------------------------------

	// Create samplers

	VkSamplerCreateInfo sampler = {};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.magFilter = VK_FILTER_LINEAR;
	sampler.minFilter = VK_FILTER_LINEAR;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	sampler.mipLodBias = 0.0f;
	sampler.compareOp = VK_COMPARE_OP_NEVER;
	sampler.minLod = 0.0f;
	sampler.maxLod = 0.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler.maxAnisotropy = 1.0;
	sampler.anisotropyEnable = VK_FALSE;

	//// Without mip mapping
	//if (vkCreateSampler(device, &sampler, nullptr, &samplers[0]) != VK_SUCCESS)
	//{
	//	throw exception("Failed to create sampler")
	//}

	// With mip mapping
	sampler.maxLod = (float) mMipLevels;
	vkCreateSampler(device, &sampler, nullptr, &mSampler);

	// With mip mapping and anisotropic filtering
	//if (vulkanDevice->features.samplerAnisotropy)
	//{
	//	sampler.maxAnisotropy = vulkanDevice->properties.limits.maxSamplerAnisotropy;
	//	sampler.anisotropyEnable = VK_TRUE;
	//}
	//VK_CHECK_RESULT(vkCreateSampler(device, &sampler, nullptr, &samplers[2]));

	// Create image view
	VkImageViewCreateInfo view = {};
	view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view.image = mImage;
	view.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view.format = format;
	view.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	view.subresourceRange.baseMipLevel = 0;
	view.subresourceRange.baseArrayLayer = 0;
	view.subresourceRange.layerCount = 1;
	view.subresourceRange.levelCount = mMipLevels;
	vkCreateImageView(device, &view, nullptr, &mImageView);




















	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;

	//renderer->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//void* data;
	//vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	//memcpy(data, pixels, static_cast<size_t>(imageSize));
	//vkUnmapMemory(device, stagingBufferMemory);

	//stbi_image_free(pixels);

	//CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mImage, mImageMemory);

	//TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//CopyBufferToImage(stagingBuffer, mImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	//TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//vkDestroyBuffer(device, stagingBuffer, nullptr);
	//vkFreeMemory(device, stagingBufferMemory, nullptr);

	//mImageView = CreateImageView(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	//CreateTextureSampler();
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

void Texture::GenerateDescriptorSetWrite(VkDescriptorSet descriptorSet,
	VkDescriptorImageInfo& imageInfo,
	VkWriteDescriptorSet& writeDescriptor)
{
	memset(&imageInfo, 0, sizeof(VkDescriptorImageInfo));
	memset(&writeDescriptor, 0, sizeof(VkWriteDescriptorSet));

	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mImageView;
	imageInfo.sampler = mSampler;

	writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptor.dstSet = descriptorSet;
	writeDescriptor.dstBinding = 1;
	writeDescriptor.dstArrayElement = 0;
	writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptor.descriptorCount = 1;
	writeDescriptor.pImageInfo = &imageInfo;
}

void Texture::CreateImage(uint32_t width,
	uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage& image,
	VkDeviceMemory& imageMemory)
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkImageCreateInfo ciImage = {};
	ciImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ciImage.imageType = VK_IMAGE_TYPE_2D;
	ciImage.extent.width = static_cast<uint32_t>(width);
	ciImage.extent.height = static_cast<uint32_t>(height);
	ciImage.extent.depth = 1;
	ciImage.mipLevels = 1;
	ciImage.arrayLayers = 1;
	ciImage.format = format;
	ciImage.tiling = tiling;
	ciImage.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	ciImage.usage = usage;
	ciImage.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ciImage.samples = VK_SAMPLE_COUNT_1_BIT;
	ciImage.flags = 0;

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

VkImageView Texture::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw exception("Failed to create texture image view");
	}

	return imageView;
}

void Texture::CreateTextureSampler()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkSamplerCreateInfo ciSampler = {};
	ciSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	ciSampler.magFilter = VK_FILTER_LINEAR;
	ciSampler.minFilter = VK_FILTER_LINEAR;
	ciSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	ciSampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	ciSampler.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	ciSampler.anisotropyEnable = VK_FALSE;
	ciSampler.maxAnisotropy = 1.0f;
	ciSampler.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	ciSampler.unnormalizedCoordinates = VK_FALSE;
	ciSampler.compareEnable = VK_FALSE;
	ciSampler.compareOp = VK_COMPARE_OP_ALWAYS;
	ciSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	ciSampler.mipLodBias = 0.0f;
	ciSampler.minLod = 0.0f;
	ciSampler.maxLod = 0.0f;

	if (vkCreateSampler(device, &ciSampler, nullptr, &mSampler) != VK_SUCCESS)
	{
		throw exception("Failed to create texture sampler");
	}
}

void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	Renderer* renderer = Renderer::Get();
	VkCommandBuffer commandBuffer = renderer->BeginSingleSubmissionCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
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
	else
	{
		throw std::invalid_argument("Unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	renderer->EndSingleSubmissionCommands(commandBuffer);
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