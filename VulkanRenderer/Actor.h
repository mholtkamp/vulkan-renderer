#pragma once

#include <vulkan/vulkan.h>

class Actor
{
public:

	Actor();

	void Draw(VkCommandBuffer commandBuffer);

	void Update(Scene* scene,
		float deltaTime);

	void Destroy();

private:

	void UpdateUniformBuffer(class Camera* camera,
		float DeltaTime);

	void CreateDescriptorSet();

	std::string mName;

	Mesh* mMesh;

	VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	VkDeviceMemory mUniformBufferMemory;


};