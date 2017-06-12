#include "Pipeline.h"

VkDescriptorSetLayout Pipeline::GetDescriptorSetLayout()
{
	return mDescriptorSetLayout;
}

void Pipeline::BindPipeline(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}
