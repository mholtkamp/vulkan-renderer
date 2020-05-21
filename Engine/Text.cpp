#include "Text.h"
#include "Renderer.h"
#include "Font.h"
#include "Vertex.h"
#include "DefaultFonts.h"

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
	mFont = &DefaultFonts::sConsolas32;
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
		Renderer* renderer = Renderer::Get();
		TextPipeline& textPipeline = renderer->GetTextPipeline();
		textPipeline.BindPipeline(commandBuffer);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer, &offset);

		VkDescriptorSet quadDescriptorSet = mDescriptorSet.GetDescriptorSet();
		vkCmdBindDescriptorSets(commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			textPipeline.GetPipelineLayout(),
			1,
			1,
			&quadDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(commandBuffer, 6 * mVisibleCharacters, 1, 0, 0);
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

void Text::SetText(std::string text)
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

	Renderer* renderer = Renderer::Get();
	VkDevice device = renderer->GetDevice();

	glm::vec2 interfaceResolution = renderer->GetInterfaceResolution();

	// Check if we need to reallocate a bigger buffer.
	size_t requiredSize = sizeof(VertexUI) * 6 * mText.size();
	if (requiredSize > mVertexBufferMemory.mSize)
	{
		DestroyVertexBuffer();
		CreateVertexBuffer();
	}

	assert(mFont);
	assert(mFont->mCharacters);

	void* data = nullptr;
	vkMapMemory(device, mVertexBufferMemory.mDeviceMemory, mVertexBufferMemory.mOffset, mVertexBufferMemory.mSize, 0, &data);

	mVisibleCharacters = 0;

	// Run through each of the characters and construct vertices for it.
	// Not using an index buffer currently, so each character is 6 vertices.
	// Topology is triangles.
	
	const char* characters = mText.c_str();
	float cursorX = 0.0f;
	float cursorY = 0.0f;

	for (int32_t i = 0; i < mText.size(); ++i)
	{
		char textChar = characters[i];
		if (textChar == '\n')
		{
			cursorY += mFont->mSize;
			cursorX = 0.0f;
			continue;
		}

		// Only ASCII is supported.
		if (textChar < ' ' ||
			textChar > '~')
		{
			continue;
		}

		Character& fontChar = mFont->mCharacters[textChar - ' '];
		VertexUI* vertices = reinterpret_cast<VertexUI*>(data) + (mVisibleCharacters * 6);

		//   0---2  3
		//   |  / / |
		//   | / /  |
		//   1  4---5
		vertices[0].mPosition.x = cursorX - fontChar.mOriginX;
		vertices[0].mPosition.y = cursorY - fontChar.mOriginY;
		vertices[0].mTexcoord.x = (float) fontChar.mX;
		vertices[0].mTexcoord.y = (float) fontChar.mY;

		vertices[1].mPosition.x = cursorX - fontChar.mOriginX;
		vertices[1].mPosition.y = cursorY - fontChar.mOriginY + fontChar.mHeight;
		vertices[1].mTexcoord.x = (float) fontChar.mX;
		vertices[1].mTexcoord.y = (float) fontChar.mY + fontChar.mHeight;

		vertices[2].mPosition.x = cursorX - fontChar.mOriginX + fontChar.mWidth;
		vertices[2].mPosition.y = cursorY - fontChar.mOriginY;
		vertices[2].mTexcoord.x = (float) fontChar.mX + fontChar.mWidth;
		vertices[2].mTexcoord.y = (float) fontChar.mY;

		vertices[3] = vertices[2]; // duplicated
		vertices[4] = vertices[1]; // duplicated

		vertices[5].mPosition.x = cursorX - fontChar.mOriginX + fontChar.mWidth;
		vertices[5].mPosition.y = cursorY - fontChar.mOriginY + fontChar.mHeight;
		vertices[5].mTexcoord.x = (float) fontChar.mX + fontChar.mWidth;
		vertices[5].mTexcoord.y = (float) fontChar.mY + fontChar.mHeight;

		for (int32_t i = 0; i < 6; ++i)
		{
			// Fill out uniform data first.
			// TODO: DELETE COLOR FROM VERTEX DATA
			vertices[i].mColor = mColor;

			// Transform texcoords into 0-1 UV space
			vertices[i].mTexcoord /= glm::vec2(mFont->mWidth, mFont->mHeight);

			//vertices[i].mPosition.x = InterfaceToNormalized(vertices[i].mPosition.x, interfaceResolution.x);
			//vertices[i].mPosition.y = InterfaceToNormalized(vertices[i].mPosition.y, interfaceResolution.y);
		}

		mVisibleCharacters++;
		cursorX += fontChar.mWidth;
	}

	vkUnmapMemory(device, mVertexBufferMemory.mDeviceMemory);

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
