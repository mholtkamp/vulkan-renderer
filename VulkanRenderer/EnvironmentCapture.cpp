#include "EnvironmentCapture.h"
#include "Constants.h"
#include "Renderer.h"
#include "Scene.h"

VkRenderPass EnvironmentCapture::sRenderPass = VK_NULL_HANDLE;

//static float sQuadVertices = {0.0f, 0.0f,
//                              }
//
EnvironmentCapture::EnvironmentCapture() :
	mDepthImage(VK_NULL_HANDLE),
	mDepthImageMemory(VK_NULL_HANDLE),
	mDepthImageView(VK_NULL_HANDLE),
	mResolution(DEFAULT_ENVIRONMENT_CAPTURE_RESOLUTION),
	mCapturedResolution(0),
	mScene(nullptr),
	mLitColorImage(VK_NULL_HANDLE),
	mLitColorImageMemory(VK_NULL_HANDLE),
	mLitColorImageView(VK_NULL_HANDLE),
	mLitColorSampler(VK_NULL_HANDLE),
	mIrradianceRenderPass(VK_NULL_HANDLE),
	mIrradianceBuffer(VK_NULL_HANDLE),
	mIrradianceBufferMemory(VK_NULL_HANDLE)
{
	for (int32_t i = 0; i < 6; ++i)
	{
		mFramebuffers[i] = VK_NULL_HANDLE;
		mIrradianceFramebuffers[i] = VK_NULL_HANDLE;
	}
}

EnvironmentCapture::~EnvironmentCapture()
{
	DestroyCubemap();
}

VkImageView EnvironmentCapture::GetFaceImageView(uint32_t index)
{
	assert(index >= 0);
	assert(index < 6);
	return mCubemap.GetFaceImageView(index);
}

VkSampler EnvironmentCapture::GetFaceSampler()
{
	return mCubemap.GetSampler();
}

VkImageView EnvironmentCapture::GetIrradianceFaceImageView(uint32_t index)
{
	assert(index >= 0);
	assert(index < 6);
	return mIrradianceCubemap.GetFaceImageView(index);
}

VkSampler EnvironmentCapture::GetIrradianceFaceSampler()
{
	return mIrradianceCubemap.GetSampler();
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

	mCubemap.TransitionToRT();
	mIrradianceCubemap.TransitionToRT();

    EarlyDepthPipeline earlyDepthPipeline;
    ReflectionlessGeometryPipeline geometryPipeline;
    LightPipeline lightPipeline;
	DirectionalLightPipeline directionalLightPipeline;
	PostProcessPipeline postProcessPipeline;

    earlyDepthPipeline.mViewportWidth = mResolution;
    earlyDepthPipeline.mViewportHeight = mResolution;
    geometryPipeline.mViewportWidth = mResolution;
    geometryPipeline.mViewportHeight = mResolution;
    lightPipeline.mViewportWidth = mResolution;
    lightPipeline.mViewportHeight = mResolution;
	directionalLightPipeline.mViewportWidth = mResolution;
	directionalLightPipeline.mViewportHeight = mResolution;
	postProcessPipeline.mViewportWidth = mResolution;
	postProcessPipeline.mViewportHeight = mResolution;

    earlyDepthPipeline.Create();
    geometryPipeline.Create();
    lightPipeline.Create();
	directionalLightPipeline.Create();
	postProcessPipeline.Create();

	mPostProcessDescriptorSet.Destroy();
	mPostProcessDescriptorSet.Create(postProcessPipeline.GetDescriptorSetLayout(1));
	mPostProcessDescriptorSet.UpdateImageDescriptor(0, mLitColorImageView, mLitColorSampler);

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

		VkClearValue clearValues[ATTACHMENT_COUNT] = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = ATTACHMENT_COUNT;
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
		directionalLightPipeline.BindPipeline(commandBuffer);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, lightPipeline.GetPipelineLayout(), 0, 1, &renderer->GetGlobalDescriptorSet(), 0, 0);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, lightPipeline.GetPipelineLayout(), 1, 1, &renderer->GetDeferredDescriptorSet(), 0, 0);
		vkCmdDraw(commandBuffer, 4, 1, 0, 0);

		lightPipeline.BindPipeline(commandBuffer);
		mScene->RenderLightVolumes(commandBuffer);

		// *******************
		//  Post Process Pass
		// *******************
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
		postProcessPipeline.BindPipeline(commandBuffer);
		VkDescriptorSet postProcessDescriptorSet = mPostProcessDescriptorSet.GetDescriptorSet();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, postProcessPipeline.GetPipelineLayout(), 1, 1, &postProcessDescriptorSet, 0, 0);
		vkCmdDraw(commandBuffer, 4, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		renderer->EndSingleSubmissionCommands(commandBuffer);

		++i;
	}

	mCubemap.SetLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR); // Override cached layout because it was modified by renderpass.
	mCubemap.TransitionToSRV(); // The cubemap needs to be blurred to create the irradiance cubemap, so we need this as an SRV now.
	mIrradianceCubemap.TransitionToRT(); // Rendering to irradiance map now.

	RenderIrradiance();

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

void EnvironmentCapture::RenderIrradiance()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	IrradianceConvolutionPipeline irradiancePipeline;
	irradiancePipeline.mViewportWidth = IRRADIANCE_RESOLUTION;
	irradiancePipeline.mViewportHeight = IRRADIANCE_RESOLUTION;
	irradiancePipeline.mRenderpass = mIrradianceRenderPass;
	irradiancePipeline.mSubpass = 0;
	irradiancePipeline.Create();

	mIrradianceDescriptorSet.Destroy();
	mIrradianceDescriptorSet.Create(irradiancePipeline.GetDescriptorSetLayout(1), 0);
	mIrradianceDescriptorSet.UpdateImageDescriptor(0, mCubemap.GetImageView(), mCubemap.GetSampler());

	const float piDiv2 = glm::radians(90.0f);
	const float pi = 3.14159265f;

	glm::mat4 rotationMatrices[6];

	rotationMatrices[0] = glm::rotate(glm::mat4(), piDiv2, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrices[1] = glm::rotate(glm::mat4(), -piDiv2, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrices[2] = glm::rotate(glm::mat4(), -piDiv2, glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrices[3] = glm::rotate(glm::mat4(), piDiv2, glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrices[4]; // Identity
	rotationMatrices[5] = glm::rotate(glm::mat4(), pi, glm::vec3(0.0f, 1.0f, 0.0f));

	VkExtent2D renderAreaExtent = {};
	renderAreaExtent.width = IRRADIANCE_RESOLUTION;
	renderAreaExtent.height = IRRADIANCE_RESOLUTION;

	for (int32_t i = 0; i < 6; ++i)
	{
		mIrradianceDescriptorSet.UpdateUniformDescriptor(1, mIrradianceBuffer, sizeof(glm::mat4));

		void* data = nullptr;
		vkMapMemory(device, mIrradianceBufferMemory, 0, sizeof(glm::mat4), 0, &data);
		memcpy(data, &rotationMatrices[i], sizeof(glm::mat4));
		vkUnmapMemory(device, mIrradianceBufferMemory);

		VkCommandBuffer commandBuffer = renderer->BeginSingleSubmissionCommands();

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = mIrradianceRenderPass;
		renderPassInfo.framebuffer = mIrradianceFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = renderAreaExtent;

		VkClearValue clearValues[1] = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		clearValues[0].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		irradiancePipeline.BindPipeline(commandBuffer);

		VkDescriptorSet irradianceDescriptor = mIrradianceDescriptorSet.GetDescriptorSet();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, irradiancePipeline.GetPipelineLayout(), 1, 1, &irradianceDescriptor, 0, nullptr);
		vkCmdDraw(commandBuffer, 4, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		renderer->EndSingleSubmissionCommands(commandBuffer);

	}
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

	if (mCubemap.IsValid())
	{
		mCubemap.Destroy();

		assert(mDepthImage != VK_NULL_HANDLE);
		assert(mDepthImageMemory != VK_NULL_HANDLE);
		assert(mDepthImageView != VK_NULL_HANDLE);

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
	assert(!mCubemap.IsValid());

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	mCubemap.Create(mResolution, mResolution, renderer->GetSwapchainFormat());
	mIrradianceCubemap.Create(IRRADIANCE_RESOLUTION, IRRADIANCE_RESOLUTION);

	CreateDepthImage();

	CreateLitColorImage();

	vkDeviceWaitIdle(device);

	CreateIrradianceRenderPass();

	CreateIrradianceFramebuffers();

	CreateIrradianceUniformBuffer();
}

glm::vec3 EnvironmentCapture::GetPosition()
{
	return mPosition;
}

void EnvironmentCapture::CreateIrradianceUniformBuffer()
{
	Renderer* renderer = Renderer::Get();
	renderer->CreateBuffer(sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, mIrradianceBuffer, mIrradianceBufferMemory);
}

void EnvironmentCapture::CreateIrradianceFramebuffers()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	for (uint32_t i = 0; i < 6; ++i)
	{
		std::vector<VkImageView> attachments;
		attachments.push_back(mIrradianceCubemap.GetFaceImageView(i));

		VkFramebufferCreateInfo ciFramebuffer = {};
		ciFramebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		ciFramebuffer.renderPass = mIrradianceRenderPass;
		ciFramebuffer.attachmentCount = attachments.size();
		ciFramebuffer.pAttachments = attachments.data();
		ciFramebuffer.width = IRRADIANCE_RESOLUTION;
		ciFramebuffer.height = IRRADIANCE_RESOLUTION;
		ciFramebuffer.layers = 1;

		if (vkCreateFramebuffer(device, &ciFramebuffer, nullptr, &mIrradianceFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::exception("Failed to create framebuffer.");
		}
	}
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

TextureCube* EnvironmentCapture::GetIrradianceMap()
{
	return &mIrradianceCubemap;
}

void EnvironmentCapture::CreateFramebuffers()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	VkRenderPass renderPass = renderer->GetRenderPass();

	for (uint32_t i = 0; i < 6; ++i)
	{
		std::vector<VkImageView> attachments;
		attachments.push_back(mCubemap.GetFaceImageView(i));
		attachments.push_back(mDepthImageView);

		for (uint32_t g = 0; g < mGBuffer.GetImageViews().size(); ++g)
		{
			attachments.push_back(mGBuffer.GetImageViews()[g]);
		}

		attachments.push_back(mLitColorImageView);

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

void EnvironmentCapture::CreateIrradianceRenderPass()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mIrradianceRenderPass == VK_NULL_HANDLE)
	{
		VkAttachmentDescription attachmentDesc = {};
		attachmentDesc.format = VK_FORMAT_R8G8B8A8_UNORM;
		attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference attachmentRef = {};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &attachmentRef;

		VkRenderPassCreateInfo ciRenderPass = {};
		ciRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		ciRenderPass.attachmentCount = 1;
		ciRenderPass.pAttachments = &attachmentDesc;
		ciRenderPass.subpassCount = 1;
		ciRenderPass.pSubpasses = &subpass;

		if (vkCreateRenderPass(device, &ciRenderPass, nullptr, &mIrradianceRenderPass) != VK_SUCCESS)
		{
			throw std::exception("Failed to create renderpass");
		}
	}
}

void EnvironmentCapture::CreateLitColorImage()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	Texture::CreateImage(mResolution,
		mResolution,
		VK_FORMAT_R16G16B16A16_SFLOAT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mLitColorImage,
		mLitColorImageMemory);

	mLitColorImageView = Texture::CreateImageView(mLitColorImage,
		VK_FORMAT_R16G16B16A16_SFLOAT,
		VK_IMAGE_ASPECT_COLOR_BIT);

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

	if (vkCreateSampler(device, &ciSampler, nullptr, &mLitColorSampler) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture sampler");
	}

	vkDeviceWaitIdle(device);
}

void EnvironmentCapture::UpdateDesriptorSet(VkDescriptorSet descriptorSet)
{
	VkDevice device = Renderer::Get()->GetDevice();

	VkDescriptorImageInfo imageInfo = {};
	VkWriteDescriptorSet descriptorWrite = {};

	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mCubemap.GetImageView();
	imageInfo.sampler = mCubemap.GetSampler();

	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = AD_TEXTURE_ENVIRONMENT;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}