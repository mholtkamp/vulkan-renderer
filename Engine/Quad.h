#pragma once

#include "Widget.h"
#include "Texture.h"
#include "Vertex.h"
#include "glm/glm.hpp"

class Quad : public Widget
{
	Quad();
	virtual ~Quad();

	virtual void Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea) override;


protected:

	Texture* mTexture;
	VertexUI mVertices[4];

	VkBuffer mVertexBuffer;
};