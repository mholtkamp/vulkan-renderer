#pragma once

#include <vulkan/vulkan.h>

#include "Scene.h"
#include "PipelineConfigs.h"

class ShadowCaster
{
public:

	ShadowCaster();

    void Destroy();

	void RenderShadows(Scene* scene, VkCommandBuffer commandBuffer);

	VkImageView GetShadowMapImageView();

	VkSampler GetShadowMapSampler();

private:

	void Initialize();

	void CreateShadowRenderPass();

	void CreateShadowFramebuffer();

	void CreateShadowMapImage();

	void CreateShadowPipeline();

	VkRenderPass mShadowRenderPass;
	VkFramebuffer mShadowFramebuffer;
	ShadowCastPipeline mShadowPipeline;
	VkImage mShadowMapImage;
	Allocation mShadowMapImageMemory;
	VkImageView mShadowMapImageView;
	VkSampler mShadowMapSampler;
};