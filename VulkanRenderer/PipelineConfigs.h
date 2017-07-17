#pragma once

#include "Enums.h"
#include "Constants.h"
#include "Pipeline.h"
#include <assert.h>

class EarlyDepthPipeline : public Pipeline
{
public:

	EarlyDepthPipeline()
	{
		mRasterizerDiscard = VK_FALSE;
		mFragmentShaderPath = "";
		mVertexShaderPath = "Shaders/bin/depthShader.vert";
		mSubpass = PASS_DEPTH;

		mBlendAttachments.clear();
	}

	virtual void PopulateLayoutBindings() override
	{
		Pipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

		// Add texture sampler descriptors for each texture slot
		for (int32_t i = 0; i < SLOT_COUNT; ++i)
		{
			AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		}
	}
};

class GeometryPipeline : public Pipeline
{
public:

	GeometryPipeline()
	{
		mSubpass = PASS_GEOMETRY;
		mDepthCompareOp = VK_COMPARE_OP_EQUAL;
		mVertexShaderPath = "Shaders/bin/geometryShader.vert";
		mFragmentShaderPath = "Shaders/bin/reflectiveGeometryShader.frag";

		// Add blend states for each attachment (1 already created).
		for (int32_t i = 0; i < GB_COUNT - 1; ++i)
		{
			AddBlendAttachmentState();
		}
	}

	virtual void PopulateLayoutBindings() override
	{
		Pipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

		// Add texture sampler descriptors for each texture slot
		for (int32_t i = 0; i < SLOT_COUNT; ++i)
		{
			AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
		}
	}
};

class DeferredPipeline : public Pipeline
{
public:

	DeferredPipeline()
	{
		mVertexShaderPath = "Shaders/bin/deferredShader.vert";
		mFragmentShaderPath = "Shaders/bin/deferredShader.frag";
		mPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mCullMode = VK_CULL_MODE_NONE;
		mSubpass = PASS_DEFERRED;
		mDepthTestEnabled = VK_FALSE;
	}

	virtual void PopulateLayoutBindings() override
	{
		Pipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Position texture
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Normal texture
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Color texture
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Specular color texture
	}
};

class LightPipeline : public DeferredPipeline
{
public:

	LightPipeline()
	{
		mVertexShaderPath = "Shaders/bin/lightShader.vert";
		mFragmentShaderPath = "Shaders/bin/lightShader.frag";
		mBlendEnabled = true;
		mCullMode = VK_CULL_MODE_FRONT_BIT;

		assert(mBlendAttachments.size() > 0);

		mBlendAttachments[0].blendEnable = VK_TRUE;
		mBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD;
		mBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		mBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		mBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_MAX;
	}

	virtual void PopulateLayoutBindings() override
	{
		DeferredPipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}
};

class DebugDeferredPipeline : public DeferredPipeline
{
public:

	DebugDeferredPipeline()
	{
		mVertexShaderPath = "Shaders/bin/debugDeferredShader.vert";
		mFragmentShaderPath = "Shaders/bin/debugDeferredShader.frag";
	}

	virtual void PopulateLayoutBindings() override
	{
		DeferredPipeline::PopulateLayoutBindings();
	}
};

class BaseDebugPipeline: public DeferredPipeline
{
public:

	BaseDebugPipeline()
	{
		mFragmentShaderPath = "Shaders/bin/environmentCaptureDebug.frag";
	}

	virtual void PopulateLayoutBindings() override
	{
		DeferredPipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}

};

class EnvironmentCaptureDebugPipeline : public BaseDebugPipeline
{
public:

	EnvironmentCaptureDebugPipeline()
	{
		mFragmentShaderPath = "Shaders/bin/environmentCaptureDebug.frag";
	}

	virtual void PopulateLayoutBindings() override
	{
		BaseDebugPipeline::PopulateLayoutBindings();
	}
};

class ShadowMapDebugPipeline : public DeferredPipeline
{
public:

	ShadowMapDebugPipeline()
	{
        mFragmentShaderPath = "Shaders/bin/shadowMapDebug.frag";
	}

	virtual void PopulateLayoutBindings() override
	{
		DeferredPipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
};