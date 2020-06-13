#pragma once

#include "Widget.h"
#include "Texture.h"
#include "Vertex.h"
#include "DescriptorSet.h"
#include "glm/glm.hpp"

struct QuadUniformBuffer
{
	glm::vec4 mTint;
};

class Quad : public Widget
{
public:

	Quad();
	virtual ~Quad();

	virtual void Render(VkCommandBuffer commandBuffer) override;

	virtual void Update() override;

	void SetTexture(class Texture* texture);

	virtual void SetColor(glm::vec4 color) override;

	void SetColor(glm::vec4 colors[4]);
	void SetColor(glm::vec4 topLeft,
				  glm::vec4 topRight,
				  glm::vec4 bottomLeft,
				  glm::vec4 bottomRight);

	void SetTint(glm::vec4 tint);

protected:

	void CreateVertexBuffer();
	void CreateUniformBuffer();
	void CreateDescriptorSet();

	void DestroyVertexBuffer();
	void DestroyUniformBuffer();
	void DestroyDescriptorSet();

	void UpdateVertexBuffer();
	void UpdateUniformBuffer();
	void UpdateDescriptorSet();

	void InitVertexData();

	Texture* mTexture;
	VertexUI mVertices[4];

	VkBuffer mVertexBuffer;
	Allocation mVertexBufferMemory;

	DescriptorSet mDescriptorSet;
	VkBuffer mUniformBuffer;
	Allocation mUniformBufferMemory;

	glm::vec4 mTint;
};