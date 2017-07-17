#pragma once

#include <vulkan/vulkan.h>

#include "Scene.h"

class ShadowCaster
{
public:

	ShadowCaster();

    void Destroy();

	void RenderShadowMap(Scene* scene);

	VkImageView GetShadowMapImageView();

	VkSampler GetShadowMapSampler();

private:

	void Initialize();

	void CreateRenderPass();

	void CreateFramebuffer();

	void CreateImage();

	VkRenderPass mRenderPass;
	VkFramebuffer mFramebuffer;

	VkImage mShadowMapImage;
	VkDeviceMemory mShadowMapImageMemory;
	VkImageView mShadowMapImageView;
	VkSampler mShadowMapSampler;


};