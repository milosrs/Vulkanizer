#include "pch.h"
#include "Model.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "QueueFamilyIndices.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Model::Model(std::string objectPath) : RenderObject()
{
	this->objectFolder = objectPath.substr(0, objectPath.find_first_of("/"));
	this->mainWindow = &MainWindow::getInstance();

	if (objectPath.find(Microsoft::glTF::GLB_EXTENSION) != std::string::npos) {
		this->modelType = ModelLoadingType::glB;
		this->objectPath = gltfPath + objectPath;
	}
	else if (objectPath.find(Microsoft::glTF::GLTF_EXTENSION) != std::string::npos) {
		this->modelType = ModelLoadingType::glTF;
		this->objectPath = gltfPath + objectPath;
	}
	else if (objectPath.find(".obj") != std::string::npos) {
		this->modelType = ModelLoadingType::OBJ;
		this->objectPath = objectPath;
	}

	switch (modelType) {
		case OBJ: loadModel(); break;
		case glTF: loadGLTFModel(); break;
		default: assert(0 && "Model loading error: Invalid model loading type! Choose either OBJ, glTF or glB."); break;
	}
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

	objectPath = objectsPath + objectPath;

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

void Model::loadGLTFModel()
{
	filesystemPath = std::filesystem::path(objectPath);

	if (std::filesystem::exists(filesystemPath)) {
		std::filesystem::path absolutePath = std::filesystem::absolute(filesystemPath);
		std::unique_ptr<StreamReader> streamReader = std::make_unique<StreamReader>(absolutePath.parent_path());
		std::unique_ptr<Microsoft::glTF::GLBResourceReader> glbResourceReader = nullptr;
		std::unique_ptr<Microsoft::glTF::GLTFResourceReader> gltfResourceReader = nullptr;
		std::string schema;
		
		if (this->modelType == ModelLoadingType::glB) {
			auto stream = streamReader->GetInputStream(absolutePath.u8string());
			glbResourceReader = std::make_unique<Microsoft::glTF::GLBResourceReader>(std::move(streamReader), std::move(stream));

			std::string gltfJsonSchema = glbResourceReader->GetJson();
			glTFDocument = std::make_unique<Microsoft::glTF::Document>(Microsoft::glTF::Deserialize(gltfJsonSchema));
		}
		else if (this->modelType == ModelLoadingType::glTF) {
			auto stream = streamReader->GetInputStream(absolutePath.u8string());
			gltfResourceReader = std::make_unique<Microsoft::glTF::GLTFResourceReader>(std::move(streamReader));

			std::stringstream schemaStream;

			schemaStream << stream->rdbuf();
			schema = schemaStream.str();

			glTFDocument = std::make_unique<Microsoft::glTF::Document>(Microsoft::glTF::Deserialize(schemaStream));
		}

		if (glTFDocument != nullptr) {
			Renderer *renderer = mainWindow->getRenderer();
			std::string modelFolder = gltfPath + objectFolder + "/";

			glTFModel = std::make_unique<vkglTF::ModelInfo>();
			glTFModel->loadFromDocument(glTFDocument.get(), modelFolder,
				renderer->getDevicePTR(), renderer->getPhysicalDeviceMemoryPropertiesPTR(),
				renderer->getQueueIndices()->getQueuePTR(), mainWindow->getCommandHandler()->getCommandPool());
			this->glTFVertexBuffer = glTFModel->vertexBuffer;
			this->indexBuffer = glTFModel->indexBuffer;
		}
	}
	else {
		assert(0 && "Model Loading Error: Model does not exist.");
	}
}

void Model::draw(VkCommandBuffer cmdBuffer) {
	if (this->glTFVertexBuffer != nullptr) {
		vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(this->vertices->getIndices().size()), 1, 0, 0, 0);
	}
	else if(glTFModel != nullptr) {
		glTFModel->draw(cmdBuffer);
	}
}
