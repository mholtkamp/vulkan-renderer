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

void DeferredPipeline::PopulateLayoutBindings()
{
	//VkDescriptorSetLayoutBinding positionSamplerLayoutBinding = {};
	//positionSamplerLayoutBinding.binding = DD_TEXTURE_POSITION;
	//positionSamplerLayoutBinding.descriptorCount = 1;
	//positionSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//positionSamplerLayoutBinding.pImmutableSamplers = nullptr;
	//positionSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//VkDescriptorSetLayoutBinding normalSamplerLayoutBinding = {};
	//normalSamplerLayoutBinding.binding = DD_TEXTURE_NORMAL;
	//normalSamplerLayoutBinding.descriptorCount = 1;
	//normalSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//normalSamplerLayoutBinding.pImmutableSamplers = nullptr;
	//normalSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//VkDescriptorSetLayoutBinding colorSamplerLayoutBinding = {};
	//colorSamplerLayoutBinding.binding = DD_TEXTURE_COLOR;
	//colorSamplerLayoutBinding.descriptorCount = 1;
	//colorSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//colorSamplerLayoutBinding.pImmutableSamplers = nullptr;
	//colorSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//VkDescriptorSetLayoutBinding uniformBufferLayoutBinding = {};
	//uniformBufferLayoutBinding.binding = DD_UNIFORM_BUFFER;
	//uniformBufferLayoutBinding.descriptorCount = 1;
	//uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//uniformBufferLayoutBinding.pImmutableSamplers = nullptr;
	//uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	//VkDescriptorSetLayoutBinding bindings[] = { positionSamplerLayoutBinding,
	//	normalSamplerLayoutBinding,
	//	colorSamplerLayoutBinding,
	//	uniformBufferLayoutBinding};

	PushSet();
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT); // Uniform buffer
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Position texture
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Normal texture
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Color texture
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

void LightPipeline::PopulateLayoutBindings()
{
	DeferredPipeline::PopulateLayoutBindings();

	PushSet();
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}

#pragma endregion
