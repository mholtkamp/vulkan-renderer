#pragma once

#include "Texture.h"

class Texture2D : public Texture
{
public:

	Texture2D();

	virtual void Create(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM) override;

	virtual void Load(const std::string& path) override;

};