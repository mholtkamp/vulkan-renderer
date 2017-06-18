#pragma once

#include "Pipeline.h"

class EarlyDepthPipeline : public Pipeline
{
public:

	EarlyDepthPipeline();

	virtual void CreatePipelineLayout() override;

	virtual void CreateDescriptorSetLayout() override;

};