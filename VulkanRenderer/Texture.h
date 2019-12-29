#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include "glm/glm.hpp"

enum class TextureType
{
	Texture2D,
	TextureCube,
	Num
};

class Texture
{

public:

	Texture();

	virtual ~Texture();

	virtual void Create(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM) = 0;

	virtual void Destroy();

	virtual void Load(const std::string& path) = 0;

	virtual void Clear(glm::vec4 color);

	void SetLayout(VkImageLayout newLayout);

	void GenerateMips();

	bool IsValid() const;

	const std::string& GetName() const;

	void CreateTextureSampler();

	void TransitionLayout(VkImageLayout newLayout);

	void TransitionToSRV();

	void TransitionToRT();

	VkImageView GetImageView();

	VkSampler GetSampler();

	static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t mipLevels = 1, uint32_t layers = 1, VkImageCreateFlags = 0);

	static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels = 1, uint32_t layers = 1, TextureType type = TextureType::Texture2D);

	static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, int32_t mipLevels = 1, int32_t layerCount = 1, VkCommandBuffer commandBuffer = VK_NULL_HANDLE);

protected:

	static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	std::string mName;

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mImageView;
	VkSampler mSampler;
	VkFormat mFormat;
	VkImageLayout mLayout;

	TextureType mTextureType;

	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mMipLevels;
	uint32_t mLayers;
};