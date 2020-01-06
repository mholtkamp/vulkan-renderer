#include "PointLight.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "Constants.h"
#include "Renderer.h"

#undef min
#undef max

using namespace std;

Mesh* PointLight::sSphereMesh = nullptr;

PointLight::PointLight() : 
	mDescriptorSet(VK_NULL_HANDLE),
	mUniformBuffer(VK_NULL_HANDLE),
	mUniformBufferMemory(VK_NULL_HANDLE)
{

}

void PointLight::Create(glm::vec3 position,
	glm::vec3 color,
	float radius)
{
	mLightData.mPosition = glm::vec4(position, 1.0f);
	mLightData.mColor = glm::vec4(color, 1.0f);
	mLightData.mRadius = radius;

	mLightData.mConstantAttenuation = 1.0f;
	mLightData.mLinearAttenuation = 1.0f;
	mLightData.mQuadraticAttenuation = 0.0f;//(INVERSE_MININUM_INTENSITY - 1.0f) / (radius * radius);

	CreateDescriptorSet();
}

void PointLight::Create(const aiLight& light,
	glm::vec3 position)
{
	mLightData.mPosition = glm::vec4(position, 1.0f);
	mLightData.mColor = glm::vec4(light.mColorDiffuse.r, light.mColorDiffuse.g, light.mColorDiffuse.b, 1.0f);

	mLightData.mConstantAttenuation = light.mAttenuationConstant;
	mLightData.mLinearAttenuation = light.mAttenuationLinear;
	mLightData.mQuadraticAttenuation = light.mAttenuationQuadratic;

	float c = light.mAttenuationConstant;
	float l = light.mAttenuationLinear;
	float q = light.mAttenuationQuadratic;
	float i = glm::max(glm::max(mLightData.mColor.r, mLightData.mColor.g), mLightData.mColor.b);
	float inv = INVERSE_MININUM_INTENSITY;
	mLightData.mRadius = (-l + sqrt(l * l - 4 * q * (c - (inv) * i))) / (2 * q);

	CreateDescriptorSet();
}

void PointLight::LoadSphereMesh()
{
	DestroySphereMesh();

	sSphereMesh = new Mesh();
	sSphereMesh->LoadMesh("Engine/Meshes/Sphere_112.dae");
}

void PointLight::DestroySphereMesh()
{
	if (sSphereMesh != nullptr)
	{
		sSphereMesh->Destroy();
		delete sSphereMesh;
		sSphereMesh = nullptr;
	}
}

void PointLight::BindSphereMeshBuffers(VkCommandBuffer& commandBuffer)
{
	if (sSphereMesh != nullptr)
	{
		sSphereMesh->BindBuffers(commandBuffer);
	}
}

void PointLight::Update(Scene* scene,
	float deltaTime)
{
	UpdateUniformBuffer(scene->GetActiveCamera(), deltaTime);
}

void PointLight::Draw(VkCommandBuffer commandBuffer)
{
	if (mDescriptorSet == VK_NULL_HANDLE)
	{
		throw exception("Attempting to render point light without a valid descriptor set");
	}
	
	assert(sSphereMesh != nullptr);

	Pipeline& lightPipeline = Renderer::Get()->GetLightPipeline();

	sSphereMesh->BindBuffers(commandBuffer);

	// Set descriptor set #1!
	// Set #0 holds all of the textures that were output from the 
	// geometry pass. These texture bindings should never change between
	// light volume draw calls, so it is best to put them on a separate 
	// descriptor set.
	vkCmdBindDescriptorSets(commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		lightPipeline.GetPipelineLayout(),
		2,
		1,
		&mDescriptorSet,
		0,
		nullptr);

	vkCmdDrawIndexed(commandBuffer,
		sSphereMesh->GetNumIndices(),
		1,
		0,
		0,
		0);
}

void PointLight::SetRadius(float radius)
{
	if (radius < 0.0f)
	{
		radius = 0.1f;
	}

	mLightData.mRadius = radius;
}

void PointLight::SetColor(glm::vec3 color)
{
	mLightData.mColor = glm::vec4(color, 1.0f);
}

void PointLight::SetPosition(glm::vec3 position)
{
	mLightData.mPosition = glm::vec4(position, 1.0f);
}

void PointLight::SetVelocity(glm::vec3 velocity)
{
	mVelocity = velocity;
}

float PointLight::GetRadius()
{
	return mLightData.mRadius;
}

glm::vec3 PointLight::GetColor()
{
	return mLightData.mColor;
}

glm::vec3 PointLight::GetPosition()
{
	return mLightData.mPosition;
}

glm::vec3 PointLight::GetVelocity()
{
	return mVelocity;
}

void PointLight::CreateDescriptorSet()
{
	if (mDescriptorSet != VK_NULL_HANDLE)
	{
		throw exception("Attempting to recreate descriptor set");
	}

	CreateUniformBuffer();

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDescriptorSetLayout layouts[] = { renderer->GetLightPipeline().GetDescriptorSetLayout(2) };
		//renderer->GetLightPipeline().GetDescriptorSetLayout(1)};
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = renderer->GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(device, &allocInfo, &mDescriptorSet) != VK_SUCCESS)
	{
		throw exception("Failed to create descriptor set");
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = mUniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(LightData);

	VkDescriptorImageInfo imageInfo = {};

	VkWriteDescriptorSet descriptorWrites[1] = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = mDescriptorSet;
	descriptorWrites[0].dstBinding = LD_UNIFORM_BUFFER;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;
	descriptorWrites[0].pImageInfo = nullptr;
	descriptorWrites[0].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device, 1, descriptorWrites, 0, nullptr);
}

void PointLight::CreateUniformBuffer()
{
	Renderer* renderer = Renderer::Get();

	VkDeviceSize bufferSize = sizeof(LightData);
	renderer->CreateBuffer(bufferSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mUniformBuffer,
		mUniformBufferMemory);
}

void PointLight::UpdateUniformBuffer(Camera* camera, float deltaTime)
{
	VkDevice device = Renderer::Get()->GetDevice();

	mLightData.mWVP = glm::translate(glm::mat4(),
		glm::vec3(mLightData.mPosition.x,
			mLightData.mPosition.y,
			mLightData.mPosition.z));
	mLightData.mWVP = glm::scale(mLightData.mWVP, glm::vec3(mLightData.mRadius, mLightData.mRadius, mLightData.mRadius) * 1.05f);

	mLightData.mWVP = camera->GetViewProjectionMatrix() * mLightData.mWVP;

	void* data;
	vkMapMemory(device, mUniformBufferMemory, 0, sizeof(LightData), 0, &data);
	memcpy(data, &mLightData, sizeof(LightData));
	vkUnmapMemory(device, mUniformBufferMemory);
}
