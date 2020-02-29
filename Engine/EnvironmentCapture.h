#pragma once

#include <array>
#include "Texture.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "GBuffer.h"
#include "DescriptorSet.h"
#include "TextureCube.h"

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

	VkImageView GetIrradianceFaceImageView(uint32_t index = 0);

	VkSampler GetIrradianceFaceSampler();

	TextureCube* GetIrradianceMap();

	glm::vec3 GetPosition();



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

	TextureCube mCubemap;
	std::array<VkFramebuffer, 6> mFramebuffers;

	TextureCube mIrradianceCubemap;
	std::array<VkFramebuffer, 6> mIrradianceFramebuffers;

	VkImage mDepthImage;
	Allocation mDepthImageMemory;
	VkImageView mDepthImageView;

	VkImage mLitColorImage;
	Allocation mLitColorImageMemory;
	VkImageView mLitColorImageView;
	VkSampler mLitColorSampler;

	DescriptorSet mPostProcessDescriptorSet;
	DescriptorSet mIrradianceDescriptorSet;

	VkBuffer mIrradianceBuffer;
	Allocation mIrradianceBufferMemory;

	VkRenderPass mIrradianceRenderPass;

	uint32_t mCapturedResolution;

	glm::vec3 mPosition;

	uint32_t mResolution;

    GBuffer mGBuffer;

	class Scene* mScene;
};