#include "Pipeline.h"
#include "Renderer.h"
#include "Utilities.h"
#include <vector>

using namespace std;

Pipeline::Pipeline() :
	mPipeline(VK_NULL_HANDLE),
	mPipelineLayout(VK_NULL_HANDLE),
	mRenderpass(VK_NULL_HANDLE),
	mSubpass(0),
	mVertexShaderPath("Shaders/bin/geometryShader.vert"),
	mFragmentShaderPath("Shaders/bin/geometryShader.frag"),
	mRasterizerDiscard(VK_FALSE),
	mPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
	mPolygonMode(VK_POLYGON_MODE_FILL),
	mLineWidth(1.0f),
	mCullMode(VK_CULL_MODE_BACK_BIT),
	mFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE),
	mDepthTestEnabled(VK_TRUE),
	mDepthWriteEnabled(VK_TRUE),
	mDepthCompareOp(VK_COMPARE_OP_LESS),
	mBlendEnabled(VK_FALSE),
    mViewportWidth(0),
    mViewportHeight(0)
{
	AddBlendAttachmentState();
}

Pipeline::~Pipeline()
{

}

void Pipeline::SetVertexShader(const std::string& path)
{
	mVertexShaderPath = path;
}

void Pipeline::SetFragmentShader(const std::string& path)
{
	mFragmentShaderPath = path;
}

VkDescriptorSetLayout Pipeline::GetDescriptorSetLayout(uint32_t index)
{
	if (index > mDescriptorSetLayouts.size())
	{
		throw exception("Accessing invalid descriptor set");
	}
	return mDescriptorSetLayouts[index];
}

void Pipeline::Create()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	PopulateLayoutBindings();
	CreateDescriptorSetLayouts();

	vector<char> vertShaderCode = ReadFile(mVertexShaderPath);

	VkShaderModule vertShaderModule;

	vertShaderModule = CreateShaderModule(vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkShaderModule fragShaderModule = VK_NULL_HANDLE;

	if (mFragmentShaderPath != "")
	{
		vector<char> fragShaderCode = ReadFile(mFragmentShaderPath);
		fragShaderModule = CreateShaderModule(fragShaderCode);
	}

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
	viewport.width = (float) ((mViewportWidth == 0) ? swapchainExtent.width : mViewportWidth);
	viewport.height = (float)  ((mViewportHeight == 0) ? swapchainExtent.height : mViewportHeight);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent.width = (mViewportWidth == 0) ? swapchainExtent.width : mViewportWidth;
    scissor.extent.height = (mViewportHeight == 0) ? swapchainExtent.height : mViewportHeight;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = mRasterizerDiscard;
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

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = mDepthTestEnabled;
	depthStencil.depthWriteEnable = mDepthWriteEnabled;
	depthStencil.depthCompareOp = mDepthCompareOp;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = mBlendAttachments.size();
	colorBlending.pAttachments = mBlendAttachments.data();
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	CreatePipelineLayout();

	VkGraphicsPipelineCreateInfo ciPipeline = {};
	ciPipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	ciPipeline.stageCount = (mFragmentShaderPath == "") ? 1 : 2;
	ciPipeline.pStages = shaderStages;
	ciPipeline.pVertexInputState = &vertexInputInfo;
	ciPipeline.pInputAssemblyState = &inputAssembly;
	ciPipeline.pViewportState = &viewportState;
	ciPipeline.pRasterizationState = &rasterizer;
	ciPipeline.pMultisampleState = &multisampling;
	ciPipeline.pDepthStencilState = &depthStencil;
	ciPipeline.pColorBlendState = &colorBlending;
	ciPipeline.pDynamicState = nullptr;
	ciPipeline.layout = mPipelineLayout;
	ciPipeline.renderPass = (mRenderpass == VK_NULL_HANDLE) ? renderer->GetRenderPass() : mRenderpass;
	ciPipeline.subpass = mSubpass;
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

	if (mFragmentShaderPath != "")
	{
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
	}

	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}


void Pipeline::Destroy()
{
	VkDevice device = Renderer::Get()->GetDevice();

	vkDestroyPipeline(device, mPipeline, nullptr);
	vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);

	for (VkDescriptorSetLayout layout : mDescriptorSetLayouts)
	{
		vkDestroyDescriptorSetLayout(device, layout, nullptr);
	}

	mDescriptorSetLayouts.clear();
	mLayoutBindings.clear();
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

void Pipeline::AddBlendAttachmentState()
{
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = mBlendEnabled;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	mBlendAttachments.push_back(colorBlendAttachment);
}

void Pipeline::AddLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = type;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.stageFlags = stageFlags;
	layoutBinding.binding = mLayoutBindings.back().size();

	mLayoutBindings.back().push_back(layoutBinding);
}

void Pipeline::PushSet()
{
	mLayoutBindings.push_back(std::vector<VkDescriptorSetLayoutBinding>());
}

void Pipeline::CreatePipelineLayout()
{
	Renderer* renderer = Renderer::Get();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = mDescriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts = mDescriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;

	if (vkCreatePipelineLayout(renderer->GetDevice(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void Pipeline::CreateDescriptorSetLayouts()
{
	Renderer* renderer = Renderer::Get();

	for (uint32_t i = 0; i < mLayoutBindings.size(); ++i)
	{
		VkDescriptorSetLayoutCreateInfo ciDescriptorSetLayout = {};
		ciDescriptorSetLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		ciDescriptorSetLayout.bindingCount = mLayoutBindings[i].size();
		ciDescriptorSetLayout.pBindings = mLayoutBindings[i].data();

		mDescriptorSetLayouts.push_back(VK_NULL_HANDLE);

		if (vkCreateDescriptorSetLayout(renderer->GetDevice(),
			&ciDescriptorSetLayout,
			nullptr,
			&mDescriptorSetLayouts[i]) != VK_SUCCESS)
		{
			throw exception("Failed to create descriptor set layout");
		}
	}
}

void Pipeline::PopulateLayoutBindings()
{
	PushSet();
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}