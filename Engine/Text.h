#pragma once

#include "Widget.h"
#include "Allocator.h"
#include "DescriptorSet.h"

struct TextUniformBuffer
{
	float mX;
	float mY;
	float mCutoff;
	float mOutlineSize;

	float mSize;
	float mPadding1;
	float mPadding2;
	float mPadding3;

	int32_t mDistanceField;
	int32_t mEffect;
};

class Text : public Widget
{
public:

	Text();
	virtual ~Text();

	virtual void Create() override;
	virtual void Destroy() override;

	virtual void Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea) override;

	virtual void Update() override;

	void SetFont(struct Font* font);
	void SetColor(glm::vec4 color);
	void SetOutlineColor(glm::vec4 color);

protected:

	void CreateVertexBuffer();
	void CreateUniformBuffer();
	void CreateDescriptorSet();

	void DestroyVertexBuffer();
	void DestroyUniformBuffer();
	void DestroyDescriptorSet();

	void UpdateVertexBuffer();
	void UpdateDescriptorSet();

	void ConstructVertexData();

	Font* mFont;
	
	VkBuffer mVertexBuffer;
	Allocation mVertexBufferMemory;
	DescriptorSet mDescriptorSet;
};