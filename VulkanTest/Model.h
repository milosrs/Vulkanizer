#pragma once
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/Document.h>

#include "RenderObject.h"
#include "StreamReader.hpp"
#include "RenderObject.h"
#include "Vertices.h"
#include "glTFModel.hpp"

class MainWindow;
class Renderer;
class QueueFamilyIndices;

namespace vkglTF {
	struct Model;
};

class Model : public RenderObject
{
public:
	Model(std::string);
	~Model();
	void draw(VkCommandBuffer cmdBuffer) override;
private:
	MainWindow *mainWindow;

	void loadModel();
	void loadGLTFModel();

	std::string objectFolder;
	std::string objectPath;
	std::filesystem::path filesystemPath;
	ModelLoadingType modelType;

	std::unique_ptr<Microsoft::glTF::Document> glTFDocument = nullptr;
	std::unique_ptr<vkglTF::ModelInfo> glTFModel = nullptr;
};

