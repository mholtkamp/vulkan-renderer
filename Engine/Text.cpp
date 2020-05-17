#include "Text.h"
#include "Renderer.h"
#include "Font.h"

Text::Text() :
	mFont(nullptr),
	mVertexBuffer(VK_NULL_HANDLE)
{

}

Text::~Text()
{
	Destroy();
}

void Text::Create()
{
	Widget::Create();

	CreateVertexBuffer();
	CreateUniformBuffer();
	CreateDescriptorSet();
}

void Text::Destroy()
{
	Widget::Destroy();

	DestroyVertexBuffer();
	DestroyUniformBuffer();
	DestroyDescriptorSet();
}

void Text::Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea)
{
	// TODO

	// Should not have children
	// RenderChildren()
}

void Text::Update()
{
	if (mDirty)
	{

	}
}