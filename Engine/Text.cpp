#include "Text.h"
#include "Renderer.h"
#include "Font.h"

Text::Text() :
	mFont(nullptr),
	mCutoff(0.5f),
	mOutlineSize(0.0f),
	mSize(32.0f),
	mOutlineColor(0.0f, 0.0f, 0.0, 1.0f),
	mVisibleCharacters(0),
	mVertexBuffer(VK_NULL_HANDLE),
	mUniformBuffer(VK_NULL_HANDLE),
	mVertexBufferDirty(true)
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

	if (mText.size() > 0 && mVertexBuffer != VK_NULL_HANDLE)
	{

	}

	// Should not have children
	// RenderChildren()
}

void Text::Update()
{
	if (mVertexBufferDirty)
	{
		UpdateVertexBuffer();
		mVertexBufferDirty = false;
	}
	
	if (mDirty)
	{
		UpdateUniformBuffer();
		UpdateDescriptorSet();
		mDirty = false;
	}

	//UpdateChildren();
}

void Text::SetFont(struct Font* font)
{
	if (mFont != font)
	{
		mFont = font;
		mVertexBufferDirty = true;
		MarkDirty();
	}
}

void Text::SetOutlineColor(glm::vec4 color)
{
	if (mColor != color)
	{
		mOutlineColor = color;
		MarkDirty();
	}
}

void Text::SetSize(float size)
{
	if (mSize != size)
	{
		mSize = size;
		MarkDirty();
	}
}

void Text::SetText(std::string& text)
{
	if (mText != text)
	{
		mText = text;
		mVertexBufferDirty = true;
		MarkDirty();
	}
}

std::string& Text::GetText()
{
	return mText;
}

void Text::CreateVertexBuffer()
{
	DestroyVertexBuffer();

	if (mText.size() > 0)
	{
		Renderer* renderer = Renderer::Get();

		renderer->CreateBuffer(mText.size() * 6 * sizeof(VertexUI),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			mVertexBuffer,
			mVertexBufferMemory);
	}
}

void Text::CreateUniformBuffer()
{
	Renderer* renderer = Renderer::Get();

	VkDeviceSize bufferSize = sizeof(TextUniformBuffer);
	renderer->CreateBuffer(bufferSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mUniformBuffer,
		mUniformBufferMemory);

	UpdateUniformBuffer();
}

void Text::CreateDescriptorSet()
{
	Renderer* renderer = Renderer::Get();
	mDescriptorSet.Create(renderer->GetTextPipeline().GetDescriptorSetLayout(1));
}

void Text::DestroyVertexBuffer()
{
	if (mVertexBuffer != VK_NULL_HANDLE)
	{
		Renderer* renderer = Renderer::Get();
		VkDevice device = renderer->GetDevice();

		vkDestroyBuffer(device, mVertexBuffer, nullptr);
		mVertexBuffer = VK_NULL_HANDLE;

		Allocator::Free(mVertexBufferMemory);
	}
}

void Text::DestroyUniformBuffer()
{
	if (mUniformBuffer != VK_NULL_HANDLE)
	{
		Renderer* renderer = Renderer::Get();

		vkDestroyBuffer(renderer->GetDevice(), mUniformBuffer, nullptr);
		mUniformBuffer = VK_NULL_HANDLE;

		Allocator::Free(mUniformBufferMemory);
	}
}

void Text::DestroyDescriptorSet()
{
	mDescriptorSet.Destroy();
}

void Text::UpdateVertexBuffer()
{
	if (mFont == nullptr)
		return;

	// Check if we need to reallocate a bigger buffer.
	size_t requiredSize = sizeof(VertexUI) * 6 * mText.size();
	if (requiredSize > mVertexBufferMemory.mSize)
	{
		DestroyVertexBuffer();
		CreateVertexBuffer();
	}

	//mVisibleCharacters = TODO;
}

void Text::UpdateUniformBuffer()
{
	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();
	glm::vec2 resolution = renderer->GetInterfaceResolution();

	Rect rect = mRect;

	if (mParent != nullptr)
	{
		rect.mX += mParent->GetRect().mX;
		rect.mY += mParent->GetRect().mY;
	}

	TextUniformBuffer ubo = {};
	ubo.mX = rect.mX;
	ubo.mY = rect.mY;
	ubo.mCutoff = mCutoff;
	ubo.mOutlineSize = mOutlineSize;
	ubo.mSize = mSize;
	ubo.mPadding1 = 1337;
	ubo.mPadding2 = 1337;
	ubo.mPadding3 = 1337;
	ubo.mDistanceField = (mFont != nullptr) ? mFont->mDistanceField : false;
	ubo.mEffect = 0;

	void* data;
	vkMapMemory(device, mUniformBufferMemory.mDeviceMemory, mUniformBufferMemory.mOffset, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, mUniformBufferMemory.mDeviceMemory);
}

void Text::UpdateDescriptorSet()
{
	Renderer* renderer = Renderer::Get();
	Texture* texture = &renderer->mWhiteTexture;
	
	if (mFont != nullptr && mFont->mTexture != nullptr)
	{
		texture = mFont->mTexture;
	}

	mDescriptorSet.UpdateUniformDescriptor(0, mUniformBuffer, sizeof(TextUniformBuffer));
	mDescriptorSet.UpdateImageDescriptor(1, texture->GetImageView(), texture->GetSampler());
}
