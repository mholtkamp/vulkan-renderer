#include "DeferredPipeline.h"
#include "Renderer.h"
#include <exception>

using namespace std;

#pragma region DeferredPipeline

DeferredPipeline::DeferredPipeline()
{
	mVertexShaderPath = "Shaders/bin/deferredShader.vert";
	mFragmentShaderPath = "Shaders/bin/deferredShader.frag";
	mPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	mCullMode = VK_CULL_MODE_NONE;
	mSubpass = PASS_DEFERRED;
	mDepthTestEnabled = VK_FALSE;
}

void DeferredPipeline::CreateDescriptorSetLayout()
{
	Renderer* renderer = Renderer::Get();

	VkDescriptorSetLayoutBinding positionSamplerLayoutBinding = {};
	positionSamplerLayoutBinding.binding = DD_TEXTURE_POSITION;
	positionSamplerLayoutBinding.descriptorCount = 1;
	positionSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	positionSamplerLayoutBinding.pImmutableSamplers = nullptr;
	positionSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding normalSamplerLayoutBinding = {};
	normalSamplerLayoutBinding.binding = DD_TEXTURE_NORMAL;
	normalSamplerLayoutBinding.descriptorCount = 1;
	normalSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalSamplerLayoutBinding.pImmutableSamplers = nullptr;
	normalSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding colorSamplerLayoutBinding = {};
	colorSamplerLayoutBinding.binding = DD_TEXTURE_COLOR;
	colorSamplerLayoutBinding.descriptorCount = 1;
	colorSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	colorSamplerLayoutBinding.pImmutableSamplers = nullptr;
	colorSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding uniformBufferLayoutBinding = {};
	uniformBufferLayoutBinding.binding = DD_UNIFORM_BUFFER;
	uniformBufferLayoutBinding.descriptorCount = 1;
	uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferLayoutBinding.pImmutableSamplers = nullptr;
	uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[] = { positionSamplerLayoutBinding,
		normalSamplerLayoutBinding,
		colorSamplerLayoutBinding,
		uniformBufferLayoutBinding};

	VkDescriptorSetLayoutCreateInfo ciDescriptorSetLayout = {};
	ciDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	ciDescriptorSetLayout.bindingCount = ARRAYSIZE(bindings);
	ciDescriptorSetLayout.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(renderer->GetDevice(),
		&ciDescriptorSetLayout,
		nullptr,
		&mDescriptorSetLayout) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor set layout");
	}
}

void DeferredPipeline::CreatePipelineLayout()
{
	Renderer* renderer = Renderer::Get();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;

	if (vkCreatePipelineLayout(renderer->GetDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

#pragma endregion

#pragma region LightPipeline

LightPipeline::LightPipeline()
{
	//mPolygonMode = VK_POLYGON_MODE_LINE;
	mVertexShaderPath = "Shaders/bin/lightShader.vert";
	mFragmentShaderPath = "Shaders/bin/lightShader.frag";
	mBlendEnabled = true;
	mCullMode = VK_CULL_MODE_FRONT_BIT;

	assert(mBlendAttachments.size() > 0);

	mBlendAttachments[0].blendEnable = VK_TRUE;
	mBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD;
	mBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	mBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
	mBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_MAX;
}

void LightPipeline::CreateDescriptorSetLayout()
{
	DeferredPipeline::CreateDescriptorSetLayout();

	Renderer* renderer = Renderer::Get();

	VkDescriptorSetLayoutBinding lightDataBufferBinding = {};
	lightDataBufferBinding.binding = LD_UNIFORM_BUFFER;
	lightDataBufferBinding.descriptorCount = 1;
	lightDataBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lightDataBufferBinding.pImmutableSamplers = nullptr;
	lightDataBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo ciDescriptorSetLayout = {};
	ciDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	ciDescriptorSetLayout.bindingCount = 1;
	ciDescriptorSetLayout.pBindings = &lightDataBufferBinding;

	if (vkCreateDescriptorSetLayout(renderer->GetDevice(),
		&ciDescriptorSetLayout,
		nullptr,
		&mLightDescriptorSetLayout) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor set layout");
	}
}

void LightPipeline::CreatePipelineLayout()
{
	Renderer* renderer = Renderer::Get();

	VkDescriptorSetLayout layouts[] = { mDescriptorSetLayout, mLightDescriptorSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = LPS_COUNT;
	pipelineLayoutInfo.pSetLayouts = layouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;

	if (vkCreatePipelineLayout(renderer->GetDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

VkDescriptorSetLayout LightPipeline::GetLightDescriptorSetLayout()
{
	return mLightDescriptorSetLayout;
}

#pragma endregion
