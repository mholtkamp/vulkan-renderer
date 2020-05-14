#pragma once

#include "Widget.h"
#include "Texture.h"
#include "Vertex.h"
#include "DescriptorSet.h"
#include "glm/glm.hpp"

struct QuadUniformBuffer
{
	glm::vec4 mHighlightColor;
	float mHighlightTime;
};

class Quad : public Widget
{
	Quad();
	virtual ~Quad();

	virtual void Create() override;
	virtual void Destroy() override;

	virtual void Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea) override;

	virtual void Update() override;

	void SetTexture(class Texture* texture);

	virtual void SetColor(glm::vec4 color) override;

	void SetColor(glm::vec4 colors[4]);

protected:

	void CreateVertexBuffer();
	void CreateDescriptorSet();

	void DestroyVertexBuffer();
	void DestroyDescriptorSet();

	void InitVertexData();

	void UpdateVertexPositions();

	void UpdateVertexBuffer();

	void UpdateDescriptorSet();

	Texture* mTexture;
	VertexUI mVertices[4];

	VkBuffer mVertexBuffer;
	Allocation mVertexBufferMemory;

	DescriptorSet mDescriptorSet;
	//VkDescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	Allocation mUniformBufferMemory;
};