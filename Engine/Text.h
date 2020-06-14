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
	float mSoftness;
	float mPadding1;
	float mPadding2;

	int32_t mDistanceField;
	int32_t mEffect;
};

class Text : public Widget
{
public:

	Text();
	virtual ~Text();

	virtual void Render(VkCommandBuffer commandBuffer) override;
	virtual void Update() override;

	void SetFont(struct Font* font);
	void SetOutlineColor(glm::vec4 color);
	void SetSize(float size);

	void SetText(const std::string& text);
	const std::string& GetText() const;

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

	Font* mFont;
	std::string mText;
	float mCutoff;
	float mOutlineSize;
	float mSize;
	float mSoftness;
	glm::vec4 mOutlineColor;

	int32_t mVisibleCharacters; // ( \n excluded )

	VkBuffer mVertexBuffer;
	Allocation mVertexBufferMemory;

	VkBuffer mUniformBuffer;
	Allocation mUniformBufferMemory;

	DescriptorSet mDescriptorSet;

	bool mVertexBufferDirty;
};