#pragma once

#include <string>
#include <vulkan/vulkan.h>

class Mesh
{
public:

	Mesh();

	void Destroy();

	void Create(class aiMesh* meshData);

	void BindBuffers(VkCommandBuffer commandBuffer);

private:

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	std::string mName;
	class Material* mMaterial;

	uint32_t mNumVertices;
	uint32_t mNumFaces;

	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;
	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;

};