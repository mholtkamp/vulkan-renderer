#pragma once

#include "Pipeline.h"

class GeometryPipeline : public Pipeline
{

public:

	GeometryPipeline();
	virtual void PopulateLayoutBindings() override;
};