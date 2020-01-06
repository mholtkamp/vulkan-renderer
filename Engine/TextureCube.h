#pragma once

#include "Texture.h"

#include <vulkan/vulkan.h>
#include <array>
#include <glm/glm.hpp>

class TextureCube : public Texture
{
public:

	TextureCube();

	virtual void Destroy() override;

	virtual void Load(const std::string& path);

	virtual void Create(uint32_t width, uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM) override;

	VkImageView GetFaceImageView(uint32_t index);

private:

	void CreateFaceImageViews();

private:

	std::array<VkImageView, 6> mFaceImageViews;
};