#include "Pipeline.h"
#include "Renderer.h"
#include "Utilities.h"
#include <vector>

using namespace std;

Pipeline::Pipeline() :
	mPipeline(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mDescriptorSetLayout(VK_NULL_HANDLE),
	mVertexShaderPath("Shaders/bin/shader.vert"),
	mFragmentShaderPath("Shaders/bin/shader.frag"),
	mPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
	mPolygonMode(VK_POLYGON_MODE_FILL),
	mLineWidth(1.0f),
	mCullMode(VK_CULL_MODE_BACK_BIT),
	mFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE),
	mBlendEnabled(VK_FALSE)
{

}

Pipeline::~Pipeline()
{

}

VkDescriptorSetLayout Pipeline::GetDescriptorSetLayout()
{
	return mDescriptorSetLayout;
}

void Pipeline::Create()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	vector<char> vertShaderCode = ReadFile(mVertexShaderPath);
	vector<char> fragShaderCode = ReadFile(mFragmentShaderPath);

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	vertShaderModule = CreateShaderModule(vertShaderCode);
	fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescription = Vertex::GetAttributeDescriptions();
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = mPrimitiveTopology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkExtent2D& swapchainExtent = renderer->GetSwapchainExtent();

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchainExtent.width;
	viewport.height = (float)  swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent =  swapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = mPolygonMode;
	rasterizer.lineWidth = mLineWidth;
	rasterizer.cullMode = mCullMode;
	rasterizer.frontFace = mFrontFace;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = mBlendEnabled;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	CreatePipelineLayout();

	VkGraphicsPipelineCreateInfo ciPipeline = {};
	ciPipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	ciPipeline.stageCount = 2;
	ciPipeline.pStages = shaderStages;
	ciPipeline.pVertexInputState = &vertexInputInfo;
	ciPipeline.pInputAssemblyState = &inputAssembly;
	ciPipeline.pViewportState = &viewportState;
	ciPipeline.pRasterizationState = &rasterizer;
	ciPipeline.pMultisampleState = &multisampling;
	ciPipeline.pDepthStencilState = nullptr;
	ciPipeline.pColorBlendState = &colorBlending;
	ciPipeline.pDynamicState = nullptr;
	ciPipeline.layout = mPipelineLayout;
	ciPipeline.renderPass = renderer->GetRenderPass();
	ciPipeline.subpass = 0;
	ciPipeline.basePipelineHandle = VK_NULL_HANDLE;
	ciPipeline.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(renderer->GetDevice(),
		VK_NULL_HANDLE,
		1,
		&ciPipeline,
		nullptr,
		&mPipeline) != VK_SUCCESS)
	{
		throw exception("Failed to create graphics pipeline");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}


void Pipeline::Destroy()
{
	VkDevice device = Renderer::Get()->GetDevice();

	vkDestroyPipeline(device, mPipeline, nullptr);
	vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
}

void Pipeline::BindPipeline(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}

VkPipelineLayout Pipeline::GetPipelineLayout()
{
	return mPipelineLayout;
}

VkShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
{
	VkDevice device = Renderer::Get()->GetDevice();
	VkShaderModuleCreateInfo ciModule = {};
	ciModule.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ciModule.codeSize = code.size();

	vector<uint32_t> codeLong(code.size() / sizeof(uint32_t) + 1);
	memcpy(codeLong.data(), code.data(), code.size());
	ciModule.pCode = codeLong.data();

	VkShaderModule module;

	if (vkCreateShaderModule(device, &ciModule, nullptr, &module) != VK_SUCCESS)
	{
		throw exception("Failed to create shader module");
	}

	return module;
}
