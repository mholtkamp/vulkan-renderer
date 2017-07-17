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

	VkDescriptorSetLayout GetDescriptorSetLayout(uint32_t index = 0);

	VkPipelineLayout GetPipelineLayout();

protected:

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

	// Shader stages
	std::string mVertexShaderPath;
	std::string mFragmentShaderPath;

    // Viewport
    uint32_t mViewportWidth;
    uint32_t mViewportHeight;

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