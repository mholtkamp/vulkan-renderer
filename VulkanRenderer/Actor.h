#pragma once

#include "Mesh.h"
#include "EnvironmentCapture.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct GeometryUniformBuffer
{
	glm::mat4 mWVPMatrix;
	glm::mat4 mWorldMatrix;
	glm::mat4 mNormalMatrix;
	float mReflectivity;
};

class Actor
{
public:

	Actor();

	~Actor();

	void Create(const aiNode& node, std::vector<Mesh>& meshes);

	void Draw(VkCommandBuffer commandBuffer);

	void Update(class Scene* scene,
		float deltaTime);

	void Destroy();

	void SetEnvironmentCapture(EnvironmentCapture* environmentCapture);

private:

	void UpdateUniformBuffer(class Camera* camera,
		float DeltaTime);

	void CreateUniformBuffer();

	void CreateDescriptorSet();

	void UpdateEnvironmentSampler();

	EnvironmentCapture* mEnvironmentCapture;

	std::string mName;

	Mesh* mMesh;

	glm::mat4 mWorldMatrix;

	VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	VkDeviceMemory mUniformBufferMemory;
};