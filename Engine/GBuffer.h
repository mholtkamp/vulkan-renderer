#pragma once

#include "Enums.h"

#include <vector>
#include <vulkan/vulkan.h>

class GBuffer
{

public:

	GBuffer();

	~GBuffer();

	void Create(uint32_t width, uint32_t height);

	void Destroy();

	std::vector<VkImage>& GetImage();

	std::vector<VkImageView>& GetImageViews();

	std::vector<VkFormat>& GetFormats();

	VkSampler GetSampler();

	void CreateImages();

	void CreateSampler();

	void CreateAttachment(GBufferIndex index, VkFormat format);

private:

    uint32_t mWidth;
    uint32_t mHeight;

	std::vector<VkImage> mImages;
	std::vector<VkDeviceMemory> mImageMemory;
	std::vector<VkImageView> mImageViews;
	std::vector<VkFormat> mFormats;
	VkSampler mSampler;
};
