#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>

class DescriptorSet
{
public:

	DescriptorSet();

	~DescriptorSet();

	void Create(VkPipelineLayout layout, VkDescriptorPool pool = VK_NULL_HANDLE);

	void Destroy();

	void UpdateImageDescriptor(int32_t binding, VkImageView imageView, VkSampler sampler);

	void UpdateUniformDescriptor(int32_t binding, VkBuffer buffer, int32_t size);

	VkDescriptorSet GetDescriptorSet();

private:

	VkDescriptorSet mDescriptorSet;

	VkDescriptorPool mOwningPool;

};