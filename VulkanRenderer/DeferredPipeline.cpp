#include "DeferredPipeline.h"
#include "Renderer.h"
#include <exception>

using namespace std;

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