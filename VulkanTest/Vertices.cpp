#include "pch.h"
#include "Vertices.h"


Vertices::Vertices()
{
	vertices = { {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}} };
}

Vertices::Vertices(Vertices const & vertices)
{
	this->vertices = vertices.vertices;
}


Vertices::~Vertices()
{
}

std::vector<Vertex> Vertices::getVertices()
{
	return this->vertices;
}
