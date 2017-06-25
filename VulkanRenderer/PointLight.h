#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct LightData
{
	glm::mat4 mWVP;
	glm::vec4 mPosition;
	glm::vec4 mColor;
	float mConstantAttenuation;
	float mLinearAttenuation;
	float mQuadraticAttenuation;

	LightData() :
		mConstantAttenuation(0.0f),
		mLinearAttenuation(0.0f),
		mQuadraticAttenuation(0.0f)
	{

	}
};

class PointLight
{
public:

	PointLight();

	void Create(glm::vec3 position,
		glm::vec3 color,
		float radius);

	void Create(const aiLight& light,
				glm::vec3 position);

	void Update(class Scene* scene,
		float deltaTime);

	void Draw(VkCommandBuffer commandBuffer);

	void SetRadius(float radius);

	void SetColor(glm::vec3 color);

	void SetPosition(glm::vec3 position);

	float GetRadius();

	glm::vec3 GetColor();

	glm::vec3 GetPosition();

	static void LoadSphereMesh();

	static void DestroySphereMesh();

	static void BindSphereMeshBuffers(VkCommandBuffer& commandBuffer);

private:

	static class Mesh* sSphereMesh;

	void CreateDescriptorSet();

	void CreateUniformBuffer();

	void UpdateUniformBuffer(class Camera* camera, float deltaTime);

	LightData mLightData;

	float mRadius;

	VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	VkDeviceMemory mUniformBufferMemory;
};