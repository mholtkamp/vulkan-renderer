#pragma once

#include <array>
#include "Texture.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "GBuffer.h"
#include "DescriptorSet.h"
#include "Cubemap.h"

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

	Cubemap* GetIrradianceMap();

private:

	void CreateIrradianceRenderPass();

	void RenderIrradiance();

	void CreateIrradianceUniformBuffer();

	void CreateIrradianceFramebuffers();

	void DestroyCubemap();

	void CreateCubemap();

	void DestroyFramebuffers();

	void CreateFramebuffers();

	void CreateDepthImage();

	void SetupCaptureCameras(std::array<Camera, 6>& cameras);

    void CreateGBuffer();

    void DestroyGBuffer();

	void CreateLitColorImage();

private:

    void UpdateDeferredDescriptor();

	static VkRenderPass sRenderPass;

	Cubemap mCubemap;
	std::array<VkFramebuffer, 6> mFramebuffers;

	Cubemap mIrradianceCubemap;
	std::array<VkFramebuffer, 6> mIrradianceFramebuffers;

	VkImage mDepthImage;
	VkDeviceMemory mDepthImageMemory;
	VkImageView mDepthImageView;

	VkImage mLitColorImage;
	VkDeviceMemory mLitColorImageMemory;
	VkImageView mLitColorImageView;
	VkSampler mLitColorSampler;

	DescriptorSet mPostProcessDescriptorSet;
	DescriptorSet mIrradianceDescriptorSet;

	VkBuffer mIrradianceBuffer;
	VkDeviceMemory mIrradianceBufferMemory;

	VkRenderPass mIrradianceRenderPass;

	uint32_t mCapturedResolution;

	glm::vec3 mPosition;

	uint32_t mResolution;

    GBuffer mGBuffer;

	class Scene* mScene;
};