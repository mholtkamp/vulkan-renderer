#pragma once

#include <vulkan/vulkan.h>
#include <string>

class Pipeline
{

public:

	Pipeline();

	virtual ~Pipeline();

	void Create();

	void SetVertexShader(const std::string& path);

	void SetFragmentShader(const std::string& path);

	void AddBlendAttachmentState();

	void BindPipeline(VkCommandBuffer commandBuffer);

	VkDescriptorSetLayout GetDescriptorSetLayout();

	virtual void CreatePipelineLayout() = 0;

protected:

	VkPipeline mPipeline;
	VkPipelineLayout mPipelineLayout;
	VkDescriptorSetLayout mDescriptorSetLayout;

	// Shader stages
	std::string mVertexShaderPath;
	std::string mFragmentShaderPath;

	// Rasterizer stage
	VkPrimitiveTopology mPrimitiveTopology;
	VkPolygonMode mPolygonMode;
	float mLineWidth;
	VkCullModeFlags mCullMode;
	VkFrontFace mFrontFace;

	// Color Blend State
	VkBool32 mBlendEnabled;

	
};