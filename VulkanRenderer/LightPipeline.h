#pragma once

#include "Pipeline.h"

class LightPipeline : public Pipeline
{
public:

	LightPipeline();

	virtual void CreatePipelineLayout() override;
};