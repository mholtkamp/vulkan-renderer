#pragma once

#include "Enums.h"

#include <vector>
#include <vulkan/vulkan.h>

class GBuffer
{

public:

	GBuffer();

	~GBuffer();

	void Create();

	void Destroy();

	std::vector<VkImage>& GetImage();

	std::vector<VkImageView>& GetImageViews();

	std::vector<VkFormat>& GetFormats();

	VkSampler GetSampler();

	void CreateImages();

	void CreateSampler();

	void CreateAttachment(GBufferIndex index, VkFormat format);

private:

	std::vector<VkImage> mImages;
	std::vector<VkDeviceMemory> mImageMemory;
	std::vector<VkImageView> mImageViews;
	std::vector<VkFormat> mFormats;
	VkFramebuffer mGBuffer;
	VkSampler mGBufferSampler;
};
