#include "EnvironmentCapture.h"
#include "Constants.h"
#include "Renderer.h"
#include "Scene.h"

VkRenderPass EnvironmentCapture::sRenderPass = VK_NULL_HANDLE;

EnvironmentCapture::EnvironmentCapture() :
	mImage(VK_NULL_HANDLE),
	mImageMemory(VK_NULL_HANDLE),
	mCubemapImageView(VK_NULL_HANDLE),
	mSampler(VK_NULL_HANDLE),
	mFaceSampler(VK_NULL_HANDLE),
	mDepthImage(VK_NULL_HANDLE),
	mDepthImageMemory(VK_NULL_HANDLE),
	mDepthImageView(VK_NULL_HANDLE),
	mResolution(DEFAULT_ENVIRONMENT_CAPTURE_RESOLUTION),
	mCapturedResolution(0),
	mScene(nullptr)
{
	for (VkImageView& view : mFaceImageViews)
	{
		view = VK_NULL_HANDLE;
	}

	for (VkFramebuffer& framebuffer : mFramebuffers)
	{
		framebuffer = VK_NULL_HANDLE;
	}
}

EnvironmentCapture::~EnvironmentCapture()
{
	DestroyCubemap();
}

void EnvironmentCapture::Capture()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mCapturedResolution != mResolution)
	{
		// Destroy old texture if it exists
		DestroyCubemap();

		// Recreate cubemap at proper resolution
		CreateCubemap();
	}

    EarlyDepthPipeline earlyDepthPipeline;
    GeometryPipeline geometryPipeline;
    LightPipeline lightPipeline;

    earlyDepthPipeline.mViewportWidth = mResolution;
    earlyDepthPipeline.mViewportHeight = mResolution;
    geometryPipeline.mViewportWidth = mResolution;
    geometryPipeline.mViewportHeight = mResolution;
    lightPipeline.mViewportWidth = mResolution;
    lightPipeline.mViewportHeight = mResolution;

    earlyDepthPipeline.Create();
    geometryPipeline.Create();
    lightPipeline.Create();

    CreateGBuffer();
    CreateFramebuffers();

	VkExtent2D renderAreaExtent = {};
	renderAreaExtent.width = mResolution;
	renderAreaExtent.height = mResolution;

	std::array<Camera, 6> cameras;
	SetupCaptureCameras(cameras);

	Camera* savedCamera = mScene->GetActiveCamera();

    // Spoof the global uniform data to pretend that screen size is 
    // only mResolution x mResolution
    GlobalUniformData& globalData = renderer->GetGlobalUniformData();
    glm::vec2 trueDimensions = globalData.mScreenDimensions;
    globalData.mScreenDimensions = glm::vec2(mResolution, mResolution);

    renderer->UpdateGlobalDescriptorSet();
    UpdateDeferredDescriptor();

	uint32_t i = 0;

	for (VkFramebuffer& framebuffer : mFramebuffers)
	{
		// Update scene using new camera
		cameras[i].Update();
		mScene->SetActiveCamera(&cameras[i]);
		mScene->Update(0.0f, false);

		VkCommandBuffer commandBuffer = renderer->BeginSingleSubmissionCommands();

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderer->GetRenderPass();
		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = renderAreaExtent;

		VkClearValue clearValues[GB_COUNT + 2] = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = GB_COUNT + 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// ******************
		//  Early Depth Pass
		// ******************
		earlyDepthPipeline.BindPipeline(commandBuffer);
		mScene->RenderGeometry(commandBuffer);
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

		// ******************
		//  Geometry Pass
		// ******************
		geometryPipeline.BindPipeline(commandBuffer);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetGeometryPipeline().GetPipelineLayout(), 0, 1, &renderer->GetGlobalDescriptorSet(), 0, 0);
		mScene->RenderGeometry(commandBuffer);
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

		// ******************
		//  Light Pass
		// ******************
		lightPipeline.BindPipeline(commandBuffer);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLightPipeline().GetPipelineLayout(), 0, 1, &renderer->GetGlobalDescriptorSet(), 0, 0);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->GetLightPipeline().GetPipelineLayout(), 1, 1, &renderer->GetDeferredDescriptorSet(), 0, 0);
        mScene->RenderLightVolumes(commandBuffer);

		vkCmdEndRenderPass(commandBuffer);

		renderer->EndSingleSubmissionCommands(commandBuffer);

		++i;
	}

    // Revert the fake changes we made to global data
    globalData.mScreenDimensions = trueDimensions;
    renderer->UpdateGlobalDescriptorSet();
    renderer->UpdateDeferredDescriptorSet();

	mCapturedResolution = mResolution;

	mScene->SetActiveCamera(savedCamera);

    DestroyFramebuffers();
    DestroyGBuffer();

    earlyDepthPipeline.Destroy();
    geometryPipeline.Destroy();
    lightPipeline.Destroy();
}

void EnvironmentCapture::UpdateDeferredDescriptor()
{
    // Update image descriptors (for each gbuffer output)
    VkDescriptorImageInfo imageInfo[GB_COUNT] = {};
    VkWriteDescriptorSet descriptorWrite[GB_COUNT] = {};

    for (uint32_t i = 0; i < GB_COUNT; ++i)
    {
        imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo[i].imageView = mGBuffer.GetImageViews()[i];
        imageInfo[i].sampler = mGBuffer.GetSampler();

        descriptorWrite[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[i].dstSet = Renderer::Get()->GetDeferredDescriptorSet();
        descriptorWrite[i].dstBinding = DD_TEXTURE_POSITION + i;
        descriptorWrite[i].dstArrayElement = 0;
        descriptorWrite[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[i].descriptorCount = 1;
        descriptorWrite[i].pImageInfo = &imageInfo[i];
    }

    vkUpdateDescriptorSets(Renderer::Get()->GetDevice(), GB_COUNT, descriptorWrite, 0, nullptr);
}

void EnvironmentCapture::CreateGBuffer()
{
    mGBuffer.Create(mResolution, mResolution);
}

void EnvironmentCapture::DestroyGBuffer()
{
    mGBuffer.Destroy();
}

void EnvironmentCapture::SetupCaptureCameras(std::array<Camera, 6>& cameras)
{
	for (uint32_t i = 0; i < cameras.size(); ++i)
	{
		cameras[i].SetPosition(mPosition);
		cameras[i].SetPerspectiveSettings(90.0f, 1.0f, 0.1f, 1024.0f);
	}

    cameras[0].SetRotation(glm::vec3(0.0f, -90.0f, 0.0f));
    cameras[1].SetRotation(glm::vec3(00.0f, 90.0f, 0.0f));
    cameras[2].SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
    cameras[3].SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    cameras[4].SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    cameras[5].SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));

}

void EnvironmentCapture::SetPosition(glm::vec3 position)
{
	mPosition = position;
}

void EnvironmentCapture::SetResolution(uint32_t size)
{
	if (size == 0 ||
		size > ENVIRONMENT_CAPTURE_MAX_RESOLUTION)
	{
		throw std::exception("Environment Capture invalid resolution");
	}

	mResolution = size;
}


void EnvironmentCapture::DestroyCubemap()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mImage != VK_NULL_HANDLE)
	{
		assert(mCubemapImageView != VK_NULL_HANDLE);
		assert(mImageMemory != VK_NULL_HANDLE);
		assert(mSampler != VK_NULL_HANDLE);
		assert(mFaceSampler != VK_NULL_HANDLE);
		assert(mDepthImage != VK_NULL_HANDLE);
		assert(mDepthImageMemory != VK_NULL_HANDLE);
		assert(mDepthImageView != VK_NULL_HANDLE);

		vkDestroyImage(device, mImage, nullptr);
		mImage = VK_NULL_HANDLE;

		vkDestroyImageView(device, mCubemapImageView, nullptr);
		mCubemapImageView = VK_NULL_HANDLE;

		vkDestroySampler(device, mSampler, nullptr);
		mSampler = VK_NULL_HANDLE;

		vkFreeMemory(device, mImageMemory, nullptr);
		mImageMemory = VK_NULL_HANDLE;

		for (VkImageView& view : mFaceImageViews)
		{
			assert(view != VK_NULL_HANDLE);
			vkDestroyImageView(device, view, nullptr);
			view = VK_NULL_HANDLE;
		}

		vkDestroySampler(device, mFaceSampler, nullptr);
		mFaceSampler = VK_NULL_HANDLE;

		vkDestroyImage(device, mDepthImage, nullptr);
		mDepthImage = VK_NULL_HANDLE;

		vkFreeMemory(device, mDepthImageMemory, nullptr);
		mDepthImageMemory = VK_NULL_HANDLE;

		vkDestroyImageView(device, mDepthImageView, nullptr);
		mDepthImageView = VK_NULL_HANDLE;
	}
}

void EnvironmentCapture::CreateCubemap()
{
	assert(mImage == VK_NULL_HANDLE);
	assert(mImageMemory == VK_NULL_HANDLE);
	assert(mSampler == VK_NULL_HANDLE);
	assert(mCubemapImageView == VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	// Create optimal tiled target image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = renderer->GetSwapchainFormat();
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent = { mResolution, mResolution, 1 };
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	imageCreateInfo.arrayLayers = 6;
	imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

	if (vkCreateImage(device, &imageCreateInfo, nullptr, &mImage) != VK_SUCCESS)
	{
		throw std::exception("Failed to create cubemap image in evironment capture");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(device, mImage, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = renderer->FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS)
	{
		throw std::exception("Failed to allocate image memory for cubemap");
	}

	if (vkBindImageMemory(device, mImage, mImageMemory, 0) != VK_SUCCESS)
	{
		throw std::exception("Failed to bind memory to image for cubemap");
	}

	//Texture::TransitionImageLayout(mImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	VkSamplerCreateInfo ciSampler = {};
	ciSampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	ciSampler.magFilter = VK_FILTER_LINEAR;
	ciSampler.minFilter = VK_FILTER_LINEAR;
	ciSampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	ciSampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	ciSampler.addressModeV = ciSampler.addressModeU;
	ciSampler.addressModeW = ciSampler.addressModeU;
	ciSampler.mipLodBias = 0.0f;
	ciSampler.compareOp = VK_COMPARE_OP_ALWAYS;
	ciSampler.compareEnable = VK_FALSE;
	ciSampler.minLod = 0.0f;
	ciSampler.maxLod = 0.0f;
	ciSampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	ciSampler.maxAnisotropy = 1.0f;
	ciSampler.anisotropyEnable = VK_FALSE;

	if (vkCreateSampler(device, &ciSampler, nullptr, &mSampler) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture sampler");
	}

	if (vkCreateSampler(device, &ciSampler, nullptr, &mFaceSampler) != VK_SUCCESS)
	{
		throw std::exception("Failed to create individual face sampler.");
	}

	CreateDepthImage();

	CreateImageViews();

	vkDeviceWaitIdle(device);
}

VkImageView EnvironmentCapture::GetFaceImageView(uint32_t index)
{
	assert(index >= 0);
	assert(index < 6);
	return mFaceImageViews[index];
}

VkSampler EnvironmentCapture::GetFaceSampler()
{
	return mFaceSampler;
}

void EnvironmentCapture::CreateDepthImage()
{
	assert(mDepthImage == VK_NULL_HANDLE);
	assert(mDepthImageMemory == VK_NULL_HANDLE);
	assert(mDepthImageView == VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	Texture::CreateImage(mResolution,
		mResolution,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mDepthImage,
		mDepthImageMemory);

	mDepthImageView = Texture::CreateImageView(mDepthImage,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_ASPECT_DEPTH_BIT);

	Texture::TransitionImageLayout(mDepthImage,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	vkDeviceWaitIdle(device);
}

void EnvironmentCapture::SetScene(Scene* scene)
{
	mScene = scene;
}

void EnvironmentCapture::DestroyFramebuffers()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	for (VkFramebuffer& framebuffer : mFramebuffers)
	{
		if (framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
			framebuffer = VK_NULL_HANDLE;
		}
	}
}

void EnvironmentCapture::CreateFramebuffers()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	VkRenderPass renderPass = renderer->GetRenderPass();

	for (uint32_t i = 0; i < 6; ++i)
	{
		std::vector<VkImageView> attachments;
		attachments.push_back(mFaceImageViews[i]);
		attachments.push_back(mDepthImageView);

		for (uint32_t g = 0; g < mGBuffer.GetImageViews().size(); ++g)
		{
			attachments.push_back(mGBuffer.GetImageViews()[g]);
		}

		VkFramebufferCreateInfo ciFramebuffer = {};
		ciFramebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		ciFramebuffer.renderPass = renderer->GetRenderPass();
		ciFramebuffer.attachmentCount = attachments.size();
		ciFramebuffer.pAttachments = attachments.data();
		ciFramebuffer.width = mResolution;
		ciFramebuffer.height = mResolution;
		ciFramebuffer.layers = 1;

		if (vkCreateFramebuffer(device, &ciFramebuffer, nullptr, &mFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create framebuffer.");
		}
	}
}

void EnvironmentCapture::CreateImageViews()
{
	// Create the cubemap image view first
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = mImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = renderer->GetSwapchainFormat();
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	if (vkCreateImageView(device, &viewInfo, nullptr, &mCubemapImageView) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture image view");
	}

	// Now create individual image views 
	for (uint32_t i = 0; i < 6; ++i)
	{
		VkImageViewCreateInfo ciImageView = {};
		ciImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ciImageView.image = mImage;
		ciImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ciImageView.format = renderer->GetSwapchainFormat();
		ciImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ciImageView.subresourceRange.baseMipLevel = 0;
		ciImageView.subresourceRange.levelCount = 1;
		ciImageView.subresourceRange.baseArrayLayer = i;
		ciImageView.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &ciImageView, nullptr, &mFaceImageViews[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create texture image view");
		}
	}
}

void EnvironmentCapture::CreateRenderPass()
{
	
}

void EnvironmentCapture::UpdateDesriptorSet(VkDescriptorSet descriptorSet)
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkDescriptorImageInfo imageInfo = {};
	VkWriteDescriptorSet descriptorWrite = {};

	imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.imageView = mCubemapImageView;
	imageInfo.sampler = mSampler;

	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = AD_TEXTURE_ENVIRONMENT;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}