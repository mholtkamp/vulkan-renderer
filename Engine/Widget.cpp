#include "Widget.h"

Widget::Widget() :
	mParent(nullptr),
	mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
	mSetScissor(false),
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
	
	Destroy();
}

// Setup any resources required by the widget.
void Widget::Create()
{

}

void Widget::Destroy()
{

}

// Issue gpu commands to display the widget.
// Recursively render children.
void Widget::Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea)
{
	RenderChildren(commandBuffer, area);
}

// Refresh any data used for rendering based on this widget's state. Use dirty flag.
// Recursively update children.
void Widget::Update()
{
	UpdateChildren();
}

Rect Widget::GetRect()
{
	return mRect;
}

void Widget::SetPosition(glm::vec2 position)
{
	mRect.mX = position.x;
	mRect.mY = position.y;
	mDirty = true;
}

void Widget::SetDimensions(glm::vec2 dimensions)
{
	mRect.mWidth = dimensions.x;
	mRect.mHeight = dimensions.y;
	mDirty = true;
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

void Widget::SetScissor(VkCommandBuffer commandBuffer, Rect& area)
{
	// Set scissor to the target area.
	VkRect2D scissorRect = {};
	scissorRect.extent.width = static_cast<uint32_t>(area.mWidth);
	scissorRect.extent.height = static_cast<uint32_t>(area.mHeight);
	scissorRect.offset.x = static_cast<int32_t>(area.mX);
	scissorRect.offset.y = static_cast<int32_t>(area.mY);

	vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
}

void Widget::RenderChildren(VkCommandBuffer commandBuffer, Rect area)
{
	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		Widget* child = mChildren[i];

		Rect childArea;
		childArea.mX = area.mX + child->mRect.mX;
		childArea.mY = area.mY + child->mRect.mY;
		childArea.mWidth = child->mRect.mWidth;
		childArea.mHeight = child->mRect.mHeight;
		childArea.Clamp(area);

		child->Render(commandBuffer, childArea, area);
	}
}

void Widget::UpdateChildren()
{
	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->Update();
	}
}