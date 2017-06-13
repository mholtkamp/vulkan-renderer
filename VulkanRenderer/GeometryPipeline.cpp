#include "GeometryPipeline.h"
#include "Renderer.h"

#include <exception>

using namespace std;

GeometryPipeline::GeometryPipeline()
{

}

void GeometryPipeline::CreatePipelineLayout()
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

void GeometryPipeline::CreateDescriptorSetLayout()
{
	Renderer* renderer = Renderer::Get();

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo ciDescriptorSetLayout = {};
	ciDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	ciDescriptorSetLayout.bindingCount = 2;
	ciDescriptorSetLayout.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(renderer->GetDevice(),
		&ciDescriptorSetLayout,
		nullptr,
		&mDescriptorSetLayout) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor set layout");
	}
}