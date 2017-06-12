#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <assimp/scene.h>

#include "Material.h"

class Mesh
{
public:

	Mesh();

	void Destroy();

	void Create(const aiMesh& meshData,
		std::vector<Material>& materials);

	void BindBuffers(VkCommandBuffer commandBuffer);

	class Material* GetMaterial();

	void UpdateDescriptorSets(VkDescriptorSet descriptorSet);

	uint32_t GetNumIndices();

	uint32_t GetNumFaces();

	uint32_t GetNumVertices();

private:

	void CreateVertexBuffer(aiVector3D* positions,
							aiVector3D* texcoords,
							aiVector3D* normals);

	void CreateIndexBuffer(aiFace* faces);

	std::string mName;
	class Material* mMaterial;

	uint32_t mNumVertices;
	uint32_t mNumFaces;

	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;
	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;

};