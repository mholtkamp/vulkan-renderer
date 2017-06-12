#pragma once

#include "Pipeline.h"

class GeometryPipeline : public Pipeline
{

public:

	GeometryPipeline();

	virtual void CreateDescriptorSetLayout() override;
	virtual void CreatePipelineLayout() override;
};