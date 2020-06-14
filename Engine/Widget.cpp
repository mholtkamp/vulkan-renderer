#include "Widget.h"
#include "Renderer.h"

Widget::Widget() :
	mParent(nullptr),
	mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
	mUseScissor(false),
	mDirty(true)
{

}

Widget::~Widget()
{
	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		delete mChildren[i];
		mChildren[i] = nullptr;
	}

	mChildren.clear();
}

// Issue gpu commands to display the widget.
// Recursively render children.
void Widget::RecursiveRender(VkCommandBuffer commandBuffer)
{
	Render(commandBuffer);

	if (mUseScissor)
	{
		PushScissor(commandBuffer);
	}

	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->RecursiveRender(commandBuffer);
	}

	if (mUseScissor)
	{
		PopScissor(commandBuffer);
	}
}

void Widget::Render(VkCommandBuffer commandBuffer)
{

}

// Refresh any data used for rendering based on this widget's state. Use dirty flag.
// Recursively update children.
void Widget::RecursiveUpdate()
{
	Update();
	mDirty = false;

	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->RecursiveUpdate();
	}
}

void Widget::Update()
{
	if (mDirty)
	{
		if (mParent != nullptr)
		{
			Rect parentRect = mParent->GetAbsoluteRect();
			mAbsoluteRect.mX = parentRect.mX + mRect.mX;
			mAbsoluteRect.mY = parentRect.mY + mRect.mY;
			mAbsoluteRect.mWidth = mRect.mWidth;
			mAbsoluteRect.mHeight = mRect.mHeight;
		}
		else
		{
			mAbsoluteRect = mRect;
		}
	}
}

Rect Widget::GetRect()
{
	return mRect;
}

Rect Widget::GetAbsoluteRect()
{
	return mAbsoluteRect;
}

void Widget::SetPosition(float x, float y)
{
	mRect.mX = x;
	mRect.mY = y;
	mDirty = true;
}

void Widget::SetDimensions(float width, float height)
{
	mRect.mWidth = width;
	mRect.mHeight = height;
	mDirty = true;
}

void Widget::SetPosition(glm::vec2 position)
{
	SetPosition(position.x, position.y);
}

void Widget::SetDimensions(glm::vec2 dimensions)
{
	SetDimensions(dimensions.x, dimensions.y);
}

void Widget::SetRect(float x, float y, float width, float height)
{
	SetPosition(x, y);
	SetDimensions(width, height);
}

void Widget::SetRect(glm::vec2 position, glm::vec2 dimensions)
{
	SetPosition(position);
	SetDimensions(dimensions);
}

void Widget::SetRect(Rect rect)
{
	SetPosition(glm::vec2(rect.mX, rect.mY));
	SetDimensions(glm::vec2(rect.mWidth, rect.mHeight));
}

void Widget::SetColor(glm::vec4 color)
{
	mColor = color;
	mDirty = true;
}

void Widget::AddChild(Widget* widget)
{
	mChildren.push_back(widget);
	mChildren.back()->mParent = this;
}

Widget* Widget::RemoveChild(Widget* widget)
{
	Widget* removedWidget = nullptr;

	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		if (mChildren[i] == widget)
		{
			removedWidget = mChildren[i];
			removedWidget->mParent = nullptr;
			mChildren.erase(mChildren.begin() + i);
			break;
		}
	}

	return removedWidget;
}

Widget* Widget::RemoveChild(int32_t index)
{
	Widget* removedWidget = nullptr;

	if (index >= 0 &&
		index < mChildren.size())
	{
		removedWidget = mChildren[index];
		mChildren.erase(mChildren.begin() + index);
	}

	return removedWidget;
}

Widget* Widget::GetChild(int32_t index)
{
	return mChildren[index];
}

void Widget::MarkDirty()
{
	mDirty = true;

	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->MarkDirty();
	}
}

float Widget::InterfaceToNormalized(float interfaceCoord, float interfaceSize)
{
	return (interfaceCoord / interfaceSize) * 2.0f - 1.0f;
}

void Widget::SetScissor(VkCommandBuffer commandBuffer, Rect& area)
{
	// Area is provided in interface dimensions, so we need to convert to actual pixel dimensions
	VkExtent2D extent = Renderer::Get()->GetSwapchainExtent();
	glm::vec2 interfaceRes = Renderer::Get()->GetInterfaceResolution();
	const float xScale = extent.width / interfaceRes.x;
	const float yScale = extent.height / interfaceRes.y;

	Rect pixelRect = area;
	pixelRect.mX *= xScale;
	pixelRect.mY *= yScale;
	pixelRect.mWidth *= xScale;
	pixelRect.mHeight *= yScale;

	// Set scissor to the target area.
	VkRect2D scissorRect = {};
	scissorRect.extent.width = static_cast<uint32_t>(pixelRect.mWidth);
	scissorRect.extent.height = static_cast<uint32_t>(pixelRect.mHeight);
	scissorRect.offset.x = static_cast<int32_t>(pixelRect.mX);
	scissorRect.offset.y = static_cast<int32_t>(pixelRect.mY);

	vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
}

void Widget::PushScissor(VkCommandBuffer commandBuffer)
{
	Rect scissorRect = mAbsoluteRect;

	if (mParent != nullptr)
	{
		scissorRect.Clamp(mParent->GetAbsoluteRect());
	}

	SetScissor(commandBuffer, scissorRect);
}

void Widget::PopScissor(VkCommandBuffer commandBuffer)
{
	if (mParent != nullptr)
	{
		SetScissor(commandBuffer, mParent->GetAbsoluteRect());
	}
	else
	{
		glm::vec2 interfaceRes = Renderer::Get()->GetInterfaceResolution();

		Rect screenRect;
		screenRect.mX = 0;
		screenRect.mY = 0;
		screenRect.mWidth = interfaceRes.x;
		screenRect.mHeight = interfaceRes.y;

		SetScissor(commandBuffer, screenRect);
	}
}
