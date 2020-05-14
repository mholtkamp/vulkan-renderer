#pragma once

#include "Enums.h"
#include "Constants.h"
#include "Pipeline.h"
#include <assert.h>

#define ENGINE_SHADER_DIR "Engine/Shaders/bin/"

class EarlyDepthPipeline : public Pipeline
{
public:

	EarlyDepthPipeline()
	{
		mRasterizerDiscard = VK_FALSE;
		mFragmentShaderPath = ENGINE_SHADER_DIR "depthShader.frag";
		mVertexShaderPath = ENGINE_SHADER_DIR "depthShader.vert";
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

class ShadowCastPipeline : public EarlyDepthPipeline
{
public:

	ShadowCastPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "shadowCastShader.vert";
		mFragmentShaderPath = "";
	}

};

class GeometryPipeline : public Pipeline
{
public:

	GeometryPipeline()
	{
		mSubpass = PASS_GEOMETRY;
		mDepthCompareOp = VK_COMPARE_OP_EQUAL;
		mVertexShaderPath = ENGINE_SHADER_DIR "reflectiveGeometryShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "reflectiveGeometryShader.frag";
		//mCullMode = VK_CULL_MODE_NONE;

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

class ReflectionlessGeometryPipeline : public GeometryPipeline
{
public:

	ReflectionlessGeometryPipeline()
	{
		mFragmentShaderPath = ENGINE_SHADER_DIR "nonreflectiveGeometryShader.frag";
	}
};

class DeferredPipeline : public Pipeline
{
public:

	DeferredPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "deferredShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "deferredShader.frag";
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
        AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Metallic
        AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Roughness
        AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Shadowmap texture
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT); // Irradiance cubemap
    }
};

class LightPipeline : public DeferredPipeline
{
public:

	LightPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "lightShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "lightShader.frag";
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

class DirectionalLightPipeline : public DeferredPipeline
{
public:

    DirectionalLightPipeline()
    {
        mVertexShaderPath = ENGINE_SHADER_DIR "directionalLightShader.vert";
        mFragmentShaderPath = ENGINE_SHADER_DIR "directionalLightShader.frag";
        mBlendEnabled = true;
        mCullMode = VK_CULL_MODE_NONE;

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
		mVertexShaderPath = ENGINE_SHADER_DIR "debugDeferredShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "debugDeferredShader.frag";
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
		mFragmentShaderPath = ENGINE_SHADER_DIR "environmentCaptureDebug.frag";
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
		mFragmentShaderPath = ENGINE_SHADER_DIR "environmentCaptureDebug.frag";
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
        mFragmentShaderPath = ENGINE_SHADER_DIR "shadowMapDebug.frag";
	}

	virtual void PopulateLayoutBindings() override
	{
		DeferredPipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
};

class PostProcessPipeline : public Pipeline
{
public:

	PostProcessPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "deferredShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "tonemapShader.frag";
		mPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mCullMode = VK_CULL_MODE_NONE;
		mSubpass = PASS_POST_PROCESS;
		mDepthTestEnabled = VK_FALSE;
		mUseVertexBinding = false;
	}

	virtual void PopulateLayoutBindings() override
	{
		Pipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
};

class NullPostProcessPipeline : public PostProcessPipeline
{
public:

	NullPostProcessPipeline()
	{
		mFragmentShaderPath = ENGINE_SHADER_DIR "nullPostProcessShader.frag";
	}

};

class IrradianceConvolutionPipeline : public Pipeline
{
public:

	IrradianceConvolutionPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "irradianceConvolutionShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "irradianceConvolutionShader.frag";

        mDepthTestEnabled = false;
        mCullMode = VK_CULL_MODE_NONE;
        mBlendEnabled = false;
        mPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		mUseVertexBinding = false;
	}

	virtual void PopulateLayoutBindings() override
	{
		Pipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}
};

class QuadPipeline : public Pipeline
{
public:

	QuadPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "quadShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "quadShader.frag";
		mPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		mCullMode = VK_CULL_MODE_NONE;
		mSubpass = PASS_UI;
		mDepthTestEnabled = VK_FALSE;
		mUseVertexBinding = true;
	}

	virtual void PopulateLayoutBindings() override
	{
		Pipeline::PopulateLayoutBindings();

		PushSet();
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
};

class TextPipeline : public QuadPipeline
{
public:

	TextPipeline()
	{
		mVertexShaderPath = ENGINE_SHADER_DIR "textShader.vert";
		mFragmentShaderPath = ENGINE_SHADER_DIR "textShader.frag";
		mPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	}

	virtual void PopulateLayoutBindings() override
	{
		QuadPipeline::PopulateLayoutBindings();
	}
};