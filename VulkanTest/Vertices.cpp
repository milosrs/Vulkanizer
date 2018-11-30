#include "pch.h"
#include "Vertices.h"

Vertices::Vertices()
{
}

Vertices::Vertices(std::vector<Vertex> vertices)
{
	this->vertices = vertices;
}

Vertices::Vertices(Vertices const & vertices)
{
	this->vertices = vertices.vertices;
	this->indices = vertices.indices;
}


Vertices::~Vertices()
{
}

std::vector<Vertex> Vertices::getVertices()
{
	return this->vertices;
}

std::vector<uint16_t> Vertices::getIndices()
{
	return this->indices;
}

void Vertices::setVertices(std::vector<Vertex> vertices)
{
	this->vertices = vertices;
}

void Vertices::setIndices(std::vector<uint16_t> indices)
{
	this->indices = indices;
}
