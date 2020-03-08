#include "Canvas.h"

Canvas::Canvas()
{

}

Canvas::~Canvas()
{

}

void Canvas::Render(VkCommandBuffer commandBuffer, Rect area, Rect parentArea)
{
	SetScissor(commandBuffer, area);

	RenderChildren(commandBuffer, area);

	SetScissor(commandBuffer, parentArea);
}