#pragma once

#include <vulkan/vulkan.h>
#include <string>

class Texture
{

public:

	Texture();
	~Texture();

	void Destroy();

	void Load(const std::string& path);

	void CreateTextureSampler();

	VkImageView GetImageView();

	VkSampler GetSampler();

	void GenerateDescriptorSetWrite(VkDescriptorSet descriptorSet,
		VkDescriptorImageInfo& imageInfo,
		VkWriteDescriptorSet& writeDescriptor);

	static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private:

	static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	std::string mName;

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mImageView;
	VkSampler mSampler;
};