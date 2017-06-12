#include "Actor.h"
#include "Renderer.h"
#include "Clock.h"
#include "Scene.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <exception>

using namespace std;

Actor::Actor() :
	mDescriptorSet(VK_NULL_HANDLE),
	mUniformBuffer(VK_NULL_HANDLE),
	mUniformBufferMemory(VK_NULL_HANDLE)
{

}

void Actor::Destroy()
{

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

void Actor::Update(Scene* camera,
	float deltaTime)
{
	UpdateUniformBuffer();
}

void Actor::UpdateUniformBuffer(Camera* scene, float deltaTime)
{
	VSUniformBuffer ubo = {};
	ubo.mModel = glm::rotate(glm::mat4(), deltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mView = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.mProjection = glm::perspective(glm::radians(45.0f), ((float)mSwapchainExtent.width) / mSwapchainExtent.height, 0.1f, 100.0f);
	ubo.mProjection[1][1] *= -1.0f;

	void* data;
	vkMapMemory(mDevice, mUniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(mDevice, mUniformBufferMemory);
}