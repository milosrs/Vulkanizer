#include "pch.h"
#include "Square.h"

Square::Square(MainWindow* window, Renderer* renderer) : RenderObject(window, renderer)
{
	vertices->setVertices({
		{ { -1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { 1.0f, -1.0f, 0.0f  },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f } },
	{ { 1.0f, 1.0f, 0.0f  },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { -1.0f, 1.0f, 0.0f  },{ 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } }
	});
	this->clearValues.resize(1);
	this->clearValues[0] = { 0.2f, 0.2f, 0.2f, 1.0f };

	vertices->setIndices({ 0, 1, 2, 2, 3, 0 });
}