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
	glm::mat4 mLightWVPMatrix;
	float mReflectivity;
    float mMetallic;
    float mRoughness;
};

class Actor
{
public:

	Actor();

	~Actor();

	void Create(const aiNode& node, std::vector<Mesh>& meshes);

	virtual void Draw(VkCommandBuffer commandBuffer);

	virtual void Update(class Scene* scene,
		float deltaTime);

	virtual void Destroy();

	void SetEnvironmentCapture(EnvironmentCapture* environmentCapture);

    void UpdateEnvironmentSampler();

	glm::vec3 GetPosition();

protected:

	void UpdateUniformBuffer(class Scene* camera,
		float DeltaTime);

	void CreateUniformBuffer();

	void CreateDescriptorSet();

	EnvironmentCapture* mEnvironmentCapture;

	std::string mName;

	Mesh* mMesh;

	glm::mat4 mWorldMatrix;

	VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	Allocation mUniformBufferMemory;
};