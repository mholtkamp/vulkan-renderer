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

	void GenerateDescriptorSetWrite(VkDescriptorSet descriptorSet,
		VkDescriptorImageInfo& imageInfo,
		VkWriteDescriptorSet& writeDescriptor);

	static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	static VkImageView CreateImageView(VkImage image, VkFormat format);

private:

	static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	std::string mName;

	VkImage mTextureImage;
	VkDeviceMemory mTextureImageMemory;
	VkImageView mTextureImageView;
	VkSampler mTextureSampler;
};