#pragma once

#include "Pipeline.h"

class DeferredPipeline : public Pipeline
{
public:

	DeferredPipeline();
	virtual void PopulateLayoutBindings() override;
};

class LightPipeline : public DeferredPipeline
{
public:

	LightPipeline();
	virtual void PopulateLayoutBindings() override;
};

class DebugDeferredPipeline : public DeferredPipeline
{
public:

	DebugDeferredPipeline();
	virtual void PopulateLayoutBindings() override;
};