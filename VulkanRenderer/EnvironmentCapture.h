#pragma once

#include <array>
#include "Texture.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "GBuffer.h"
#include "DescriptorSet.h"

class EnvironmentCapture
{
public:

	EnvironmentCapture();

	~EnvironmentCapture();

	void Capture();

	void SetPosition(glm::vec3 position);

	void SetResolution(uint32_t size);

	void SetScene(class Scene* scene);

	void UpdateDesriptorSet(VkDescriptorSet descriptorSet);

	VkImageView GetFaceImageView(uint32_t index = 0);

	VkSampler GetFaceSampler();

private:

	static void CreateRenderPass();

	void DestroyCubemap();

	void CreateCubemap();

	void DestroyFramebuffers();

	void CreateFramebuffers();

	void CreateImageViews();

	void CreateDepthImage();

	void SetupCaptureCameras(std::array<Camera, 6>& cameras);

    void CreateGBuffer();

    void DestroyGBuffer();

	void CreateLitColorImage();

private:

    void UpdateDeferredDescriptor();

	static VkRenderPass sRenderPass;

	VkImage mImage;
	VkDeviceMemory mImageMemory;
	VkImageView mCubemapImageView;
	VkSampler mSampler;

	VkImage mDepthImage;
	VkDeviceMemory mDepthImageMemory;
	VkImageView mDepthImageView;

	VkImage mLitColorImage;
	VkDeviceMemory mLitColorImageMemory;
	VkImageView mLitColorImageView;
	VkSampler mLitColorSampler;

	DescriptorSet mPostProcessDescriptorSet;

	uint32_t mCapturedResolution;

	std::array<VkImageView, 6> mFaceImageViews;
	VkSampler mFaceSampler;
	std::array<VkFramebuffer, 6> mFramebuffers;

	glm::vec3 mPosition;

	uint32_t mResolution;

    GBuffer mGBuffer;

	class Scene* mScene;
};