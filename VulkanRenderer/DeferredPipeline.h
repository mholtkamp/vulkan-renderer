#pragma once

#include "Pipeline.h"

class DeferredPipeline : public Pipeline
{
public:

	DeferredPipeline();

	virtual void CreateDescriptorSetLayout() override;
	virtual void CreatePipelineLayout() override;
};