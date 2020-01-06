#include "DescriptorSet.h"
#include "Renderer.h"

using namespace std;

DescriptorSet::DescriptorSet() :
	mDescriptorSet(VK_NULL_HANDLE),
	mOwningPool(VK_NULL_HANDLE)
{

}

DescriptorSet::~DescriptorSet()
{
	Destroy();
}

void DescriptorSet::Create(VkDescriptorSetLayout layout, VkDescriptorPool pool)
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDescriptorPool owningPool = (pool == VK_NULL_HANDLE) ? renderer->GetDescriptorPool() : pool;

	VkDescriptorSetLayout layouts[] = { layout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = owningPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(device, &allocInfo, &mDescriptorSet) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor set");
	}

	mOwningPool = owningPool;
}

void DescriptorSet::Destroy()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	if (mDescriptorSet != VK_NULL_HANDLE &&
		mOwningPool != VK_NULL_HANDLE)
	{
		vkFreeDescriptorSets(device, mOwningPool, 1, &mDescriptorSet);
		mDescriptorSet = VK_NULL_HANDLE;
		mOwningPool = VK_NULL_HANDLE;
	}
}

void DescriptorSet::UpdateImageDescriptor(int32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout layout)
{
	assert(mDescriptorSet != VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = layout;
	imageInfo.imageView = imageView;
	imageInfo.sampler = sampler;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = mDescriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void DescriptorSet::UpdateUniformDescriptor(int32_t binding, VkBuffer buffer, int32_t size)
{
	assert(mDescriptorSet != VK_NULL_HANDLE);

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer;
	bufferInfo.range = size;
	bufferInfo.offset = 0;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = mDescriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

VkDescriptorSet DescriptorSet::GetDescriptorSet()
{
	return mDescriptorSet;
}