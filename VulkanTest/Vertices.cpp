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

std::vector<uint32_t> Vertices::getIndices()
{
	return this->indices;
}

void Vertices::setVertices(std::vector<Vertex> vertices)
{
	this->vertices = vertices;
}

void Vertices::setIndices(std::vector<uint32_t> indices)
{
	this->indices = indices;
}

void Vertices::addVertices(Vertex v)
{
	this->vertices.push_back(v);
}

void Vertices::addIndices(uint32_t ind)
{
	this->indices.push_back(ind);
}

void Vertices::insertVertices(concurrency::concurrent_vector<Vertex> verts)
{
	vertices.insert(vertices.begin(), verts.begin(), verts.end());
}

void Vertices::insertIndices(concurrency::concurrent_vector<uint32_t> inds)
{
	indices.insert(indices.begin(), inds.begin(), inds.end());
}
