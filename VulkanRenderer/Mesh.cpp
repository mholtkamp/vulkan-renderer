#include "Mesh.h"
#include "Renderer.h"

// Temp mesh data
const Vertex sVertices[] = { {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
							 {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
							 {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
							 {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

const uint32_t sIndices[] = { 0, 1, 2, 2, 3, 0 };

Mesh::Mesh() :
	mName("Mesh"),
	mMaterial(nullptr),
	mNumVertices(0),
	mNumFaces(0),
	mVertexBuffer(VK_NULL_HANDLE),
	mVertexBufferMemory(VK_NULL_HANDLE),
	mIndexBuffer(VK_NULL_HANDLE),
	mIndexBufferMemory(VK_NULL_HANDLE)
{

}

void Mesh::Destroy()
{
	VkDevice device = Renderer::Get()->GetDevice();

	vkDestroyBuffer(device, mIndexBuffer, nullptr);
	vkFreeMemory(device, mIndexBufferMemory, nullptr);
	vkDestroyBuffer(device, mVertexBuffer, nullptr);
	vkFreeMemory(device, mVertexBufferMemory, nullptr);
}

void Mesh::Create(class aiMesh* meshData)
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Mesh::BindBuffers(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { mVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::CreateVertexBuffer()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDeviceSize bufferSize = sizeof(sVertices);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, sVertices, (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

	renderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Mesh::CreateIndexBuffer()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDeviceSize bufferSize = sizeof(sIndices);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, sIndices, static_cast<size_t>(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);

	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

	renderer->CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}
