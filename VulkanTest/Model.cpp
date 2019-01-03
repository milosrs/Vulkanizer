#include "pch.h"
#include "Model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Model::Model(std::string objectPath, std::string texturePath, unsigned int mode, 
	MainWindow* mainWindow, Renderer* renderer) : RenderObject(mainWindow, renderer)
{
	this->objectPath = objectPath;
	this->texturePath = texturePath;
	this->mode = mode;

	loadModel();
}

Model::~Model()
{
}

void Model::loadModel()
{
	uint32_t hack = 0;
	tinyobj::attrib_t vertexAttributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	std::unordered_map<Vertex, uint32_t> uniqueVertices;

	if (!tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warn, &err, objectPath.c_str())) {
		throw new std::runtime_error(warn + " " + err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vert{};

			vert.position = {
				vertexAttributes.vertices[3 * index.vertex_index + 0],
				vertexAttributes.vertices[3 * index.vertex_index + 1],
				vertexAttributes.vertices[3 * index.vertex_index + 2]
			};

			vert.texCoord = {
				vertexAttributes.texcoords[2 * index.texcoord_index + 0],
				1.0f - vertexAttributes.texcoords[2 * index.texcoord_index + 1]
			};
			
			vert.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vert) == 0) {
				uniqueVertices[vert] = static_cast<uint32_t>(vertices->getVertices().size());
				vertices->addVertices(vert);
			}

			vertices->addIndices(uniqueVertices[vert]);
		}
	}
}

/*void Model::paralelLoadingIsBadIdea()
{
	uint32_t hack = 0;
	tinyobj::attrib_t vertexAttributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	concurrency::concurrent_unordered_map<Vertex, uint32_t> uniqueVertices;
	concurrency::concurrent_vector<Vertex> verts;
	concurrency::concurrent_vector<uint32_t> inds;

	if (!tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &warn, &err, objectPath.c_str())) {
		throw new std::runtime_error(warn + " " + err);
	}

	Concurrency::parallel_for_each(shapes.begin(), shapes.end(), [&](auto shape) {
		std::vector<tinyobj::index_t> indices = shape.mesh.indices;

		Concurrency::parallel_for_each(indices.begin(), indices.end(), [&](auto index) {
			Vertex vert{};

			vert.position = {
				vertexAttributes.vertices[3 * index.vertex_index + 0],
				vertexAttributes.vertices[3 * index.vertex_index + 1],
				vertexAttributes.vertices[3 * index.vertex_index + 2]
			};

			vert.texCoord = {
				vertexAttributes.texcoords[2 * index.texcoord_index + 0],
				1.0f - vertexAttributes.texcoords[2 * index.texcoord_index + 1]
			};

			vert.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vert) == 0) {
				uniqueVertices[vert] = static_cast<uint32_t>(verts.size());
				verts.push_back(vert);
			}

			inds.push_back(uniqueVertices[vert]);
		});
	});

	vertices->insertVertices(verts);
	vertices->insertIndices(inds);
}
*/