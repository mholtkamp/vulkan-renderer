#include "Canvas.h"
#include "Renderer.h"

Canvas::Canvas()
{

}

Canvas::~Canvas()
{

}

void Canvas::Render(VkCommandBuffer commandBuffer)
{
	Rect scissorRect = mAbsoluteRect;
	
	if (mParent != nullptr)
	{
		mAbsoluteRect.Clamp(mParent->GetAbsoluteRect());
	}

	SetScissor(commandBuffer, mAbsoluteRect);

	RenderChildren(commandBuffer);

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