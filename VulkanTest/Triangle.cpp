#include "pch.h"
#include "Triangle.h"


Triangle::Triangle(MainWindow* window, Renderer* renderer, CommandBuffer* cmdBuffer, CommandPool* cmdPool) : Scene(window, renderer, cmdBuffer, cmdPool)
{
}

void Triangle::render(VkViewport* viewport) {

}

void Triangle::recordFrameBuffer(CommandBuffer *, MainWindow *)
{
	std::cout << "Recording..." << std::endl;
}
