#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class Pipeline
{

public:

	Pipeline();

	virtual ~Pipeline();

	void Create();

	void Destroy();

	void SetVertexShader(const std::string& path);

	void SetFragmentShader(const std::string& path);

	void BindPipeline(VkCommandBuffer commandBuffer);

	VkDescriptorSetLayout GetDescriptorSetLayout();

	VkPipelineLayout GetPipelineLayout();

protected:

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	virtual void CreateDescriptorSetLayout() = 0;
	virtual void CreatePipelineLayout() = 0;

	void AddBlendAttachmentState();

	VkPipeline mPipeline;
	VkPipelineLayout mPipelineLayout;
	VkDescriptorSetLayout mDescriptorSetLayout;
	uint32_t mSubpass;

	// Shader stages
	std::string mVertexShaderPath;
	std::string mFragmentShaderPath;

	// Rasterizer stage
	VkBool32 mRasterizerDiscard;
	VkPrimitiveTopology mPrimitiveTopology;
	VkPolygonMode mPolygonMode;
	float mLineWidth;
	VkCullModeFlags mCullMode;
	VkFrontFace mFrontFace;

	// Depth Stencil state
	VkBool32 mDepthTestEnabled;
	VkBool32 mDepthWriteEnabled;
	VkCompareOp mDepthCompareOp;

	// Color Blend State
	VkBool32 mBlendEnabled;
	std::vector<VkPipelineColorBlendAttachmentState> mBlendAttachments;
};