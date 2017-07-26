#pragma once

#include <stdint.h>
#include <vulkan/vulkan.h>

class DescriptorSet
{
public:

	DescriptorSet();

	~DescriptorSet();

	void Create(VkDescriptorSetLayout layout, VkDescriptorPool pool = VK_NULL_HANDLE);

	void Destroy();

	void UpdateImageDescriptor(int32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	void UpdateUniformDescriptor(int32_t binding, VkBuffer buffer, int32_t size);

	VkDescriptorSet GetDescriptorSet();

private:

	VkDescriptorSet mDescriptorSet;

	VkDescriptorPool mOwningPool;

};