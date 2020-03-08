#include "Widget.h"

Widget::Widget() :
	mParent(nullptr),
	mSetScissor(false)
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

void Widget::AddChild(Widget* widget)
{
	mChildren.push_back(widget);
}

Widget* Widget::RemoveChild(Widget* widget)
{
	Widget* removedWidget = nullptr;

	for (int32_t i = 0; i < mChildren.size(); ++i)
	{
		if (mChildren[i] == widget)
		{
			removedWidget = mChildren[i];
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

void Widget::Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea)
{

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