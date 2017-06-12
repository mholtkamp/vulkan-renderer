#pragma once

#include <vulkan/vulkan.h>

class Actor
{
public:

	Actor();

private:

	void CreateDescriptorSet();

	VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	VkDeviceMemory mUniformBufferMemory;

	Mesh* mMesh;
};