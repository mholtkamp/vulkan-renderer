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
	PushScissor(commandBuffer);

	RenderChildren(commandBuffer);

	PopScissor(commandBuffer);
}