#pragma once
#ifndef VERTICES_H
#define VERTICES_H
#endif
#include "PLATFORM.h"
#include <vector>
#include <array>

struct Vertex {
	glm::vec2 position;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription;

		bindingDescription.binding = 0;									//Imamo samo jedan vektor pun ovih struktura, tako da cemo raditi binding samo jednom. Ovo je index tog bajndovanja.
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;		//Pomeri se na sledeci Vertex posle svakog vertexa. Ne koristimo instance rendering, tako da koristimo ovo.
		bindingDescription.stride = sizeof(Vertex);						//Razmak u bajtovima izmedju svake instance Vertex strukture

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		//Za vec2 inPosition
		attributeDescriptions[0].binding = 0;							//Iz kog indexa bindovanja dobavljamo ove podatke
		attributeDescriptions[0].location = 0;							//Ovaj podatak dobijamo iz vertex shadera
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		//Za vec3 inColor
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

class Vertices
{
private:
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
public:
	Vertices();
	Vertices(std::vector<Vertex>);
	Vertices(Vertices const& vertices);
	~Vertices();

	std::vector<Vertex> getVertices();
	std::vector<uint16_t> getIndices();
	void setVertices(std::vector<Vertex> vertices);
	void setIndices(std::vector<uint16_t> indices);
};

