#pragma once

#include <array>
#include "Texture.h"
#include <glm/glm.hpp>

class EnvironmentCapture
{
public:

	EnvironmentCapture();

	~EnvironmentCapture();

	void Capture();

	void SetPosition(glm::vec3 position);

	void SetTextureResolution(uint32_t size);

private:

	static void CreateRenderPass();

	void DestroyCubemap();

	void CreateCubemap();

	void DestroyFramebuffers();

	void CreateFramebuffers();

	void CreateImageViews();

	void CreateDepthImage();

private:

	static VkRenderPass sRenderPass;

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mCubemapImageView;
	VkSampler mSampler;

	VkImage mDepthImage;
	VkDeviceMemory mDepthImageMemory;
	VkImageView mDepthImageView;

	uint32_t mCapturedResolution;

	std::array<VkImageView, 6> mFaceImageViews;
	std::array<VkFramebuffer, 6> mFramebuffers;

	glm::vec3 mPosition;

	uint32_t mResolution;

};