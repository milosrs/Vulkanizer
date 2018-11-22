#include "pch.h"
#include "Triangle.h"


Triangle::Triangle(MainWindow* window, Renderer* renderer) : Scene(window, renderer)
{
}

void Triangle::render(VkViewport* viewport) {

}

void Triangle::recordFrameBuffer(CommandBuffer *, MainWindow *)
{
	std::cout << "Recording..." << std::endl;
}
