// Model converter that converts art assets to game-ready assets
// to be used by VulkanRenderer.

// Output File Format: (.vrm)
//
//
//

#define NAME_LENGTH 32

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>
#include <iostream>

void GetMaterialInfo(const aiScene* scene)
{
	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];

		aiString name;
		material->Get(AI_MATKEY_NAME, name);

		aiColor3D diffuseColor;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

		aiColor3D specularColor;
		material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

		uint32_t numDiffuseTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
		uint32_t numUnknownTextures = material->GetTextureCount(aiTextureType_UNKNOWN);
		uint32_t numNoneTextures = material->GetTextureCount(aiTextureType_NONE);
		uint32_t numEmissiveTextures = material->GetTextureCount(aiTextureType_EMISSIVE);
		uint32_t numAmbientTextures = material->GetTextureCount(aiTextureType_AMBIENT);

		aiString texPath;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
		std::string path(texPath.C_Str());
		std::cout << path;
	}
}

void WriteMeshFile(aiMesh* mesh)
{
	std::string name = mesh->mName.C_Str();

	if (name == "")
	{
		printf("No name\n");
		name = "null";
	}

	name += ".vrm";

	FILE* file = fopen(name.c_str(), "wb");

	if (file == nullptr)
	{
		printf("Failed to open file for write.\n");
		return;
	}

	assert(mesh->HasNormals());

	name.resize(NAME_LENGTH);

	fwrite(name.c_str(), 1, NAME_LENGTH, file);
	fwrite(&mesh->mNumVertices, sizeof(uint32_t), 1, file);
	fwrite(&mesh->mNumFaces, sizeof(uint32_t), 1, file);
	fwrite(&mesh->mMaterialIndex, sizeof(uint32_t), 1, file);
	fwrite(mesh->mVertices, sizeof(aiVector3D), mesh->mNumVertices, file);

	if (mesh->HasTextureCoords(0))
	{
		// Assume 2D texture. Store only U/V floats
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			fwrite(mesh->mTextureCoords[0] + i, 2 * sizeof(float), 1, file);
		}
	}
	else
	{
		// Set all UVs to 0.0, 0.0
		uint32_t zeroBufferSize = 2 * sizeof(float) * mesh->mNumVertices;
		char* zeroBuffer = (char*) malloc(zeroBufferSize);
		memset(zeroBuffer, 0, zeroBufferSize);
		fwrite(zeroBuffer, zeroBufferSize, 1, file);
		free(zeroBuffer);
		zeroBuffer = nullptr;
	}

	fwrite(mesh->mNormals, sizeof(aiVector3D), mesh->mNumVertices, file);

	for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
	{
		// Enforce triangulated faces
		assert(mesh->mFaces[i].mNumIndices == 3);
		fwrite(&mesh->mFaces[i].mIndices, sizeof(uint32_t), 3, file);
	}

	fclose(file);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Invalid number of arguments. Should only be one filename as argument.\n");
		return 1;
	}

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(argv[1], aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		WriteMeshFile(scene->mMeshes[i]);
	}

	GetMaterialInfo(scene);

	scene;

	if (scene->HasLights())
	{
		//WriteLightFile();
	}
}