#pragma once

#include "Widget.h"
#include "Texture.h"
#include "Vertex.h"
#include "DescriptorSet.h"
#include "glm/glm.hpp"

struct QuadUniformBuffer
{
	glm::vec4 mPadding; // Temp placeholder
};

class Quad : public Widget
{
public:

	Quad();
	virtual ~Quad();

	virtual void Create() override;
	virtual void Destroy() override;

	virtual void Render(VkCommandBuffer commandBuffer) override;

	virtual void Update() override;

	void SetTexture(class Texture* texture);

	virtual void SetColor(glm::vec4 color) override;

	void SetColor(glm::vec4 colors[4]);

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
};