#include "EarlyDepthPipeline.h"
#include "Renderer.h"
#include <exception>

using namespace std;

EarlyDepthPipeline::EarlyDepthPipeline()
{
	mRasterizerDiscard = VK_FALSE;
	mFragmentShaderPath = "";
	mSubpass = PASS_DEPTH;

	mBlendAttachments.clear();
}

void EarlyDepthPipeline::PopulateLayoutBindings()
{
	PushSet();
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

	// Add texture sampler descriptors for each texture slot
	for (int32_t i = 0; i < SLOT_COUNT; ++i)
	{
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
}