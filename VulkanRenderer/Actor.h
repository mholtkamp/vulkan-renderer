#pragma once

#include "Mesh.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct GeometryUniformBuffer
{
	glm::mat4 mWVPMatrix;
	glm::mat4 mWorldMatrix;
	glm::mat4 mNormalMatrix;
};

class Actor
{
public:

	Actor();

	void Create(const aiNode& node, std::vector<Mesh>& meshes);

	void Draw(VkCommandBuffer commandBuffer);

	void Update(class Scene* scene,
		float deltaTime);

	void Destroy();

private:

	void UpdateUniformBuffer(class Camera* camera,
		float DeltaTime);

	void CreateUniformBuffer();

	void CreateDescriptorSet();

	std::string mName;

	Mesh* mMesh;

	glm::mat4 mWorldMatrix;

	VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	VkDeviceMemory mUniformBufferMemory;
};