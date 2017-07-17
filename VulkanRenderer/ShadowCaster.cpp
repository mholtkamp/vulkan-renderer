#include "ShadowCaster.h"
#include "Renderer.h"

ShadowCaster::ShadowCaster() :
	mRenderPass(VK_NULL_HANDLE),
	mFramebuffer(VK_NULL_HANDLE),
	mShadowMapImage(VK_NULL_HANDLE),
	mShadowMapImageMemory(VK_NULL_HANDLE),
	mShadowMapImageView(VK_NULL_HANDLE),
	mShadowMapSampler(VK_NULL_HANDLE)
{

}

ShadowCaster::~ShadowCaster()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mRenderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device, mRenderPass, nullptr);
		mRenderPass = VK_NULL_HANDLE;

		vkDestroyFramebuffer(device, mFramebuffer, nullptr);
		mFramebuffer = VK_NULL_HANDLE;

		vkDestroyImage(device, mShadowMapImage, nullptr);
		mShadowMapImage = VK_NULL_HANDLE;

		vkDestroyImageView(device, mShadowMapImageView, nullptr);
		mShadowMapImageView = VK_NULL_HANDLE;

		vkDestroySampler(device, mShadowMapSampler, nullptr);
		mShadowMapSampler = VK_NULL_HANDLE;

		vkFreeMemory(device, mShadowMapImageMemory, nullptr);
		mShadowMapImageMemory = VK_NULL_HANDLE;
	}
}

void ShadowCaster::RenderShadowMap(Scene* scene)
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (scene == nullptr)
	{
		throw std::exception("Attempting to render shadow map for null scene");
	}

	if (mRenderPass == VK_NULL_HANDLE)
	{
		Initialize();
	}

	// Create temp pipeline
	EarlyDepthPipeline pipeline;
	pipeline.mViewportWidth = SHADOW_MAP_RESOLUTION;
	pipeline.mViewportHeight = SHADOW_MAP_RESOLUTION;
	pipeline.mRenderpass = mRenderPass;
	pipeline.mVertexShaderPath = "Shaders/bin/shadowMapShader.vert";
	pipeline.Create();

	VkExtent2D renderAreaExtent = {};
	renderAreaExtent.width = SHADOW_MAP_RESOLUTION;
	renderAreaExtent.height = SHADOW_MAP_RESOLUTION;

	VkCommandBuffer commandBuffer = renderer->BeginSingleSubmissionCommands();

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = mRenderPass;
	renderPassInfo.framebuffer = mFramebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = renderAreaExtent;

	VkClearValue clearValue = {};
	clearValue.color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValue.depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	pipeline.BindPipeline(commandBuffer);
	scene->RenderGeometry(commandBuffer);
	
	vkCmdEndRenderPass(commandBuffer);
	renderer->EndSingleSubmissionCommands(commandBuffer);

	pipeline.Destroy();
}

VkImageView ShadowCaster::GetShadowMapImageView()
{
	return mShadowMapImageView;
}

VkSampler ShadowCaster::GetShadowMapSampler()
{
	return mShadowMapSampler;
}

void ShadowCaster::Initialize()
{
	CreateImage();
	CreateRenderPass();
	CreateFramebuffer();
}

void ShadowCaster::CreateRenderPass()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkAttachmentDescription attachmentDesc = {};
	attachmentDesc.format = VK_FORMAT_D16_UNORM;
	attachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkAttachmentReference attachmentRef = {};
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;
	subpass.pDepthStencilAttachment = &attachmentRef;

	VkRenderPassCreateInfo ciRenderPass = {};
	ciRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	ciRenderPass.attachmentCount = 1;
	ciRenderPass.pAttachments = &attachmentDesc;
	ciRenderPass.subpassCount = 1;
	ciRenderPass.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &ciRenderPass, nullptr, &mRenderPass) != VK_SUCCESS)
	{
		throw std::exception("Failed to create renderpass");
	}
}

void ShadowCaster::CreateFramebuffer()
{
	assert(mRenderPass != VK_NULL_HANDLE);
	assert(mShadowMapImageView != VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkFramebufferCreateInfo ciFramebuffer = {};
	ciFramebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	ciFramebuffer.renderPass = mRenderPass;
	ciFramebuffer.attachmentCount = 1;
	ciFramebuffer.pAttachments = &mShadowMapImageView;
	ciFramebuffer.width = SHADOW_MAP_RESOLUTION;
	ciFramebuffer.height = SHADOW_MAP_RESOLUTION;
	ciFramebuffer.layers = 1;

	if (vkCreateFramebuffer(device, &ciFramebuffer, nullptr, &mFramebuffer) != VK_SUCCESS)
	{
		throw std::exception("Failed to create framebuffer.");
	}
}

void ShadowCaster::CreateImage()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	Texture::CreateImage(SHADOW_MAP_RESOLUTION,
		SHADOW_MAP_RESOLUTION,
		VK_FORMAT_D16_UNORM,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		mShadowMapImage,
		mShadowMapImageMemory);

	mShadowMapImageView = Texture::CreateImageView(mShadowMapImage,
		VK_FORMAT_D16_UNORM,
		VK_IMAGE_ASPECT_DEPTH_BIT);

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

	if (vkCreateSampler(device, &ciSampler, nullptr, &mShadowMapSampler) != VK_SUCCESS)
	{
		throw std::exception("Failed to create texture sampler");
	}

	vkDeviceWaitIdle(device);
}
