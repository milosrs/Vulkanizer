#include "pch.h"
#include "Square.h"

Square::Square(MainWindow* window, Renderer* renderer) : Scene(window, renderer)
{
	this->vertices = std::make_shared<Vertices>();
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