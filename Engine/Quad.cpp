#include "Quad.h"
#include "Renderer.h"

Quad::Quad() :
	mTexture(nullptr),
	mVertexBuffer(VK_NULL_HANDLE)
{
	CreateVertexBuffer();

}

Quad::~Quad()
{
	Destroy();
}

void Quad::Create()
{
	Widget::Create();

	InitVertexData();
	CreateVertexBuffer();
	CreateUniformBuffer();
	CreateDescriptorSet();
}

void Quad::Destroy()
{
	Widget::Destroy();

	DestroyVertexBuffer();
	DestroyUniformBuffer();
	DestroyDescriptorSet();
}

void Quad::Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea)
{
	// Make sure to bind the quad pipeline. Quad and text rendering will be interleaved.
	Renderer* renderer = Renderer::Get();
	QuadPipeline& quadPipeline = renderer->GetQuadPipeline();
	quadPipeline.BindPipeline(commandBuffer);

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer, &offset);

	VkDescriptorSet quadDescriptorSet = mDescriptorSet.GetDescriptorSet();
	vkCmdBindDescriptorSets(commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		quadPipeline.GetPipelineLayout(),
		1,
		1,
		&quadDescriptorSet,
		0,
		nullptr);

	vkCmdDraw(commandBuffer, 4, 1, 0, 0);

	RenderChildren(commandBuffer, area);
}

void Quad::Update()
{
	if (mDirty)
	{
		UpdateVertexPositions();
		UpdateVertexBuffer();
		UpdateDescriptorSet();
		mDirty = false;
	}

	UpdateChildren();
}

void Quad::SetTexture(class Texture* texture)
{
	mTexture = texture;
	mDirty = true;
}

void Quad::SetColor(glm::vec4 color)
{
	for (int32_t i = 0; i < 4; ++i)
	{
		mVertices[i].mColor = color;
	}

	mDirty = true;
}

void Quad::SetColor(glm::vec4 colors[4])
{
	for (int32_t i = 0; i < 4; ++i)
	{
		mVertices[i].mColor = colors[i];
	}

	mDirty = true;
}

void Quad::CreateVertexBuffer()
{
	DestroyVertexBuffer();

	Renderer* renderer = Renderer::Get();

	renderer->CreateBuffer(4 * sizeof(VertexUI),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mVertexBuffer,
		mVertexBufferMemory);
}

void Quad::CreateUniformBuffer()
{
	Renderer* renderer = Renderer::Get();

	VkDeviceSize bufferSize = sizeof(QuadUniformBuffer);
	renderer->CreateBuffer(bufferSize,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		mUniformBuffer,
		mUniformBufferMemory);
}

void Quad::CreateDescriptorSet()
{
	Renderer* renderer = Renderer::Get();
	mDescriptorSet.Create(renderer->GetQuadPipeline().GetDescriptorSetLayout(1));
}

void Quad::DestroyVertexBuffer()
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

void Quad::DestroyUniformBuffer()
{
	if (mUniformBuffer != VK_NULL_HANDLE)
	{
		Renderer* renderer = Renderer::Get();

		vkDestroyBuffer(renderer->GetDevice(), mUniformBuffer, nullptr);
		mUniformBuffer = VK_NULL_HANDLE;

		Allocator::Free(mUniformBufferMemory);
	}
}

void Quad::DestroyDescriptorSet()
{
	mDescriptorSet.Destroy();
}

void Quad::UpdateVertexPositions()
{
	Renderer* renderer = Renderer::Get();
	glm::vec2 resolution = renderer->GetInterfaceResolution();

	Rect rect = mRect;

	if (mParent != nullptr)
	{
		rect.mX += mParent->GetRect().mX;
		rect.mY += mParent->GetRect().mY;
	}

	mVertices[0].mPosition.x = Widget::InterfaceToNormalized(mRect.mX, resolution.x);
	mVertices[0].mPosition.y = Widget::InterfaceToNormalized(mRect.mY, resolution.y);

	mVertices[1].mPosition.x = Widget::InterfaceToNormalized(mRect.mX, resolution.x);
	mVertices[1].mPosition.y = Widget::InterfaceToNormalized(mRect.mY + mRect.mHeight, resolution.y);

	mVertices[2].mPosition.x = Widget::InterfaceToNormalized(mRect.mX + mRect.mWidth, resolution.x);
	mVertices[2].mPosition.y = Widget::InterfaceToNormalized(mRect.mY, resolution.y);

	mVertices[3].mPosition.x = Widget::InterfaceToNormalized(mRect.mX + mRect.mWidth, resolution.x);
	mVertices[3].mPosition.y = Widget::InterfaceToNormalized(mRect.mY + mRect.mHeight, resolution.y);
}

void Quad::InitVertexData()
{
	mVertices[0].mPosition.x = 0.0f;
	mVertices[0].mPosition.y = 0.0f;
	mVertices[0].mTexcoord.x = 0.0f;
	mVertices[0].mTexcoord.y = 0.0f;
	mVertices[0].mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	mVertices[1].mPosition.x = 0.0f;
	mVertices[1].mPosition.y = 1.0f;
	mVertices[1].mTexcoord.x = 0.0f;
	mVertices[1].mTexcoord.y = 1.0f;
	mVertices[1].mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	mVertices[2].mPosition.x = 1.0f;
	mVertices[2].mPosition.y = 0.0f;
	mVertices[2].mTexcoord.x = 1.0f;
	mVertices[2].mTexcoord.y = 0.0f;
	mVertices[2].mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	mVertices[3].mPosition.x = 1.0f;
	mVertices[3].mPosition.y = 1.0f;
	mVertices[3].mTexcoord.x = 1.0f;
	mVertices[3].mTexcoord.y = 1.0f;
	mVertices[3].mColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Quad::UpdateVertexBuffer()
{
	VkDevice device = Renderer::Get()->GetDevice();

	void* data = nullptr;
	vkMapMemory(device, mVertexBufferMemory.mDeviceMemory, mVertexBufferMemory.mOffset, mVertexBufferMemory.mSize, 0, &data);
	memcpy(data, mVertices, sizeof(VertexUI) * 4);
	vkUnmapMemory(device, mVertexBufferMemory.mDeviceMemory);
}

void Quad::UpdateDescriptorSet()
{
	Renderer* renderer = Renderer::Get();
	Texture* texture = (mTexture != nullptr) ? mTexture : &renderer->mWhiteTexture;

	mDescriptorSet.UpdateUniformDescriptor(0, mUniformBuffer, sizeof(QuadUniformBuffer));
	mDescriptorSet.UpdateImageDescriptor(1, texture->GetImageView(), texture->GetSampler());
}