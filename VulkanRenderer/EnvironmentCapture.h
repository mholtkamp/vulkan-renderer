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

	void DestroyFramebuffer();

	void CreateFramebuffer();

	void CreateImageViews();

private:

	static VkRenderPass sRenderPass;

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mCubemapImageView;
	VkSampler mSampler;
	uint32_t mCapturedResolution;

	std::array<VkImageView, 6> mFaceImageViews;
	std::array<VkFramebuffer, 6> mFramebuffers;

	glm::vec3 mPosition;

	uint32_t mResolution;

};