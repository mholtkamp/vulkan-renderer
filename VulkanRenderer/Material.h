#pragma once

#include <glm/glm.hpp>
#include <string>

class Texture;

class Material
{
public:

	Material();

	void Destory();


private:

	std::string mName;

	glm::vec3 mDiffuseColor;
	glm::vec3 mSpecularColor;

	Texture* mDiffuseTexture;
	Texture* mSpecularTexture;
	Texture* mNormalTexture;
	Texture* mReflectiveTexture;
	Texture* mEmissiveTexture;
};