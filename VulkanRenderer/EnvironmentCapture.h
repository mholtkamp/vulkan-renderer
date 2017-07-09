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

private:

	static VkRenderPass sRenderPass;

	VkFramebuffer mFramebuffer;

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mImageView;
	VkSampler mSampler;
	uint32_t mCapturedResolution;

	glm::vec3 mPosition;

	uint32_t mResolution;

};