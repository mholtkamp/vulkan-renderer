#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <assimp/scene.h>

#include "Material.h"
#include "Allocator.h"

class Mesh
{
public:

	Mesh();

	~Mesh();

	void Destroy();

	void Create(const aiMesh& meshData,
		std::vector<Material>* materials = nullptr);

	void BindBuffers(VkCommandBuffer commandBuffer);

	class Material* GetMaterial();

	void SetMaterial(class Material* newMaterial);

	void UpdateDescriptorSets(VkDescriptorSet descriptorSet);

	void LoadMesh(const std::string& path);

	uint32_t GetNumIndices();

	uint32_t GetNumFaces();

	uint32_t GetNumVertices();

private:

	void CreateVertexBuffer(aiVector3D* positions,
							aiVector3D* texcoords,
							aiVector3D* normals,
							aiVector3D* tangents);

	void CreateIndexBuffer(aiFace* faces);

	std::string mName;
	bool mOwnsMaterial;
	class Material* mMaterial;

	uint32_t mNumVertices;
	uint32_t mNumFaces;

	VkBuffer mVertexBuffer;
	Allocation mVertexBufferMemory;
	VkBuffer mIndexBuffer;
	Allocation mIndexBufferMemory;

};