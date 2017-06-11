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

void WriteMeshFile(aiMesh* mesh)
{
	std::string name = mesh->mName.C_Str();

	if (name == "")
	{
		printf("No name\n");
	}

	name += ".vrm";

	FILE* file = fopen(name.c_str(), "wb");

	if (file == nullptr)
	{
		printf("Failed to open file for write.\n");
		return;
	}

	name.resize(NAME_LENGTH);

	fwrite(name.c_str(), 1, NAME_LENGTH, file);
	fwrite(&mesh->mNumVertices, sizeof(uint32_t), 1, file);
	fwrite(&mesh->mNumFaces, sizeof(uint32_t), 1, file);
	fwrite(&mesh->mVertices, sizeof(aiVector3D), mesh->mNumVertices, file);
	fwrite(&mesh->mTextureCoords[0], sizeof(aiVector3D), mesh->mNumVertices, file);
	fwrite(&mesh->mNormals, sizeof(aiVector3D), mesh->mNumVertices, file);

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

	const aiScene* scene = importer.ReadFile(argv[1], //aiProcess_CalcTangentSpace |
		aiProcess_Triangulate
/*		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType*/);

	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		WriteMeshFile(scene->mMeshes[i]);
	}

	if (scene->HasLights())
	{
		//WriteLightFile();
	}
}