#pragma once

#include "Pipeline.h"

class DeferredPipeline : public Pipeline
{
public:

	DeferredPipeline();

	virtual void CreateDescriptorSetLayout() override;
	virtual void CreatePipelineLayout() override;
};

class LightPipeline : public DeferredPipeline
{
public:
	LightPipeline();

	void CreateDescriptorSetLayout() override;
	void CreatePipelineLayout() override;

	VkDescriptorSetLayout GetLightDescriptorSetLayout();

protected:

	VkDescriptorSetLayout mLightDescriptorSetLayout;
};
