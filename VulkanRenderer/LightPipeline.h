#pragma once

#include "Pipeline.h"

class LightPipeline : public Pipeline
{
public:

	LightPipeline();

	virtual void CreateDescriptorSetLayout() override;
	virtual void CreatePipelineLayout() override;
};