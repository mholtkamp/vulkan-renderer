#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

#include "Vertex.h"

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

	VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t index = 0);

	VkPipelineLayout GetPipelineLayout();

protected:

	void CreateGraphicsPipeline();
	void CreateComputePipeline();

	void PushSet();
	void AddLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	virtual void PopulateLayoutBindings();
	void CreateDescriptorSetLayouts();
	void CreatePipelineLayout();

	void AddBlendAttachmentState();

	VkPipeline mPipeline;
	VkPipelineLayout mPipelineLayout;
	std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
	
public:

	VkRenderPass mRenderpass;
    uint32_t mSubpass;
	bool mComputePipeline;

	// Shader stages
	std::string mVertexShaderPath;
	std::string mFragmentShaderPath;
	std::string mComputeShaderPath;

    // Viewport
    uint32_t mViewportWidth;
    uint32_t mViewportHeight;

	// Vertex Input
	bool mUseVertexBinding; // (Generally only false for post process / full screen quad pipelines.)
	VertexType mVertexType;

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

	std::vector<std::vector<VkDescriptorSetLayoutBinding> > mLayoutBindings;
};