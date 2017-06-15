#include "Actor.h"
#include "Renderer.h"
#include "Clock.h"
#include "Scene.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <exception>

using namespace std;

Actor::Actor() :
	mName("Actor"),
	mMesh(nullptr),
	mDescriptorSet(VK_NULL_HANDLE),
	mUniformBuffer(VK_NULL_HANDLE),
	mUniformBufferMemory(VK_NULL_HANDLE)
{

}

void Actor::Create(const aiNode& node, vector<Mesh>& meshes)
{
	mName = node.mName.C_Str();
	aiMatrix4x4 invTransform = node.mTransformation;
	invTransform.Transpose();
	memcpy(&mWorldMatrix, &invTransform, sizeof (aiMatrix4x4));

	if (node.mNumMeshes > 0)
	{
		if (node.mNumMeshes != 1)
		{
			printf("Multiple meshes received by one actor.\n");
		}

		mMesh = &meshes[node.mMeshes[0]];

		CreateUniformBuffer();
		CreateDescriptorSet();
	}
}

void Actor::Destroy()
{
	VkDevice device = Renderer::Get()->GetDevice();

	vkDestroyBuffer(device, mUniformBuffer, nullptr);
	vkFreeMemory(device, mUniformBufferMemory, nullptr);
}

void Actor::CreateUniformBuffer()
{
	Renderer* renderer = Renderer::Get();

	VkDeviceSize bufferSize = sizeof(GeometryUniformBuffer);
	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mUniformBuffer, mUniformBufferMemory);
}

void Actor::CreateDescriptorSet()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDescriptorSetLayout layouts[] = { renderer->GetGeometryPipeline().GetDescriptorSetLayout() };
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
	bufferInfo.range = sizeof(mUniformBuffer);

	VkDescriptorImageInfo imageInfo = {};

	VkWriteDescriptorSet descriptorWrites[1] = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = mDescriptorSet;
	descriptorWrites[0].dstBinding = AD_UNIFORM_BUFFER;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;
	descriptorWrites[0].pImageInfo = nullptr;
	descriptorWrites[0].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device, 1, descriptorWrites, 0, nullptr);

	mMesh->UpdateDescriptorSets(mDescriptorSet);
}

void Actor::Draw(VkCommandBuffer commandBuffer)
{
	Pipeline& geometryPipeline = Renderer::Get()->GetGeometryPipeline();

	if (mMesh != nullptr)
	{
		mMesh->BindBuffers(commandBuffer);

		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			geometryPipeline.GetPipelineLayout(),
			0,
			1,
			&mDescriptorSet,
			0,
			nullptr);

		vkCmdDrawIndexed(commandBuffer,
			mMesh->GetNumIndices(),
			1,
			0,
			0,
			0);
	}
}

void Actor::Update(Scene* scene,
	float deltaTime)
{
	UpdateUniformBuffer(scene->GetActiveCamera(), deltaTime);
}

void Actor::UpdateUniformBuffer(Camera* camera, float deltaTime)
{
	VkDevice device = Renderer::Get()->GetDevice();

	GeometryUniformBuffer ubo = {};
	ubo.mWVPMatrix = camera->GetViewProjectionMatrix() * mWorldMatrix;

	void* data;
	vkMapMemory(device, mUniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, mUniformBufferMemory);
}