#pragma once

#include <vulkan/vulkan.h>
#include <array>

class Cubemap
{
public:

	Cubemap();

	~Cubemap();

	void Destroy();

	void Create(uint32_t resolution, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

	bool IsValid();

	VkImageView GetFaceImageView(uint32_t index);

	VkImageView GetCubemapImageView();

	VkSampler GetSampler();

private:

	void CreateImage();

	void CreateImageViews();

	void CreateSampler();

private:

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mCubemapImageView;
	VkSampler mSampler;
	std::array<VkImageView, 6> mFaceImageViews;

	VkFormat mFormat;
	uint32_t mResolution;
};