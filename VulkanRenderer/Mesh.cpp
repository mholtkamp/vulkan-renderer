#include "Mesh.h"
#include "Renderer.h"
#include "Vertex.h"
#include <assimp/scene.h>

// Temp mesh data
//const Vertex sVertices[] = { {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
//							 {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
//							 {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
//							 {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };
//
//const uint32_t sIndices[] = { 0, 1, 2, 2, 3, 0 };

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

void Mesh::Create(const aiMesh& meshData,
	std::vector<Material>& materials)
{
	mNumVertices = meshData.mNumVertices;
	mNumFaces = meshData.mNumFaces;

	if (mNumVertices == 0 ||
		mNumFaces == 0)
	{
		return;
	}

	// Get pointers to vertex attributes
	aiVector3D* positions = meshData.mVertices;
	aiVector3D* texcoords3D = meshData.mTextureCoords[0];
	aiVector3D* normals = meshData.mNormals;

	aiFace* faces = meshData.mFaces;

	CreateVertexBuffer(positions, texcoords3D, normals);
	CreateIndexBuffer(faces);

	// Assign associated material
	mMaterial = &materials[meshData.mMaterialIndex];
}

void Mesh::BindBuffers(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { mVertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

Material* Mesh::GetMaterial()
{
	return mMaterial;
}

void Mesh::UpdateDescriptorSets(VkDescriptorSet descriptorSet)
{
	mMaterial->UpdateDescriptorSets(descriptorSet);
}

uint32_t Mesh::GetNumIndices()
{
	return mNumFaces * 3;
}

uint32_t Mesh::GetNumFaces()
{
	return mNumFaces;
}

uint32_t Mesh::GetNumVertices()
{
	return mNumVertices;
}

void Mesh::CreateVertexBuffer(aiVector3D* positions,
							  aiVector3D* texcoords,
							  aiVector3D* normals)
{
	Vertex* vertices = static_cast<Vertex*>(malloc(sizeof(Vertex) * mNumVertices));

	// Create an interleaved VBO
	for (uint32_t i = 0; i < mNumVertices; ++i)
	{
		vertices[i].mPosition = glm::vec3(positions[i].x,
										  positions[i].y,
										  positions[i].z);
		vertices[i].mTexcoord = glm::vec2(texcoords[i].x,
										  texcoords[i].y);
		vertices[i].mNormal = glm::vec3(normals[i].x,
										normals[i].y,
										normals[i].z);
	}

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDeviceSize bufferSize = sizeof(Vertex) * mNumVertices;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices, (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

	renderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	free(vertices);
}

void Mesh::CreateIndexBuffer(aiFace* faces)
{
	uint32_t* indices = static_cast<uint32_t*>(malloc(mNumFaces * 3 * sizeof(uint32_t)));

	for (uint32_t i = 0; i < mNumFaces; ++i)
	{
		// Enforce triangulated faces
		assert(faces[i].mNumIndices == 3);
		indices[i * 3 + 0] = faces[i].mIndices[0];
		indices[i * 3 + 1] = faces[i].mIndices[1];
		indices[i * 3 + 2] = faces[i].mIndices[2];
	}

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	VkDeviceSize bufferSize = mNumFaces * 3 * sizeof(uint32_t);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices, static_cast<size_t>(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);

	renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

	renderer->CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	free(indices);
}
