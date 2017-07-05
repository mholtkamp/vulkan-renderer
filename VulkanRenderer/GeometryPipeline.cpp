#include "GeometryPipeline.h"
#include "Renderer.h"

#include <exception>

using namespace std;

GeometryPipeline::GeometryPipeline()
{
	mSubpass = PASS_GEOMETRY;
	mDepthCompareOp = VK_COMPARE_OP_EQUAL;

	// Add blend states for each attachment (1 already created).
	for (int32_t i = 0; i < GB_COUNT - 1; ++i)
	{
		AddBlendAttachmentState();
	}
}

void GeometryPipeline::PopulateLayoutBindings()
{
	PushSet();
	AddLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

	// Add texture sampler descriptors for each texture slot
	for (int32_t i = 0; i < SLOT_COUNT; ++i)
	{
		AddLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	}
}