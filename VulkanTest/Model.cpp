#include "pch.h"
#include "Model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Model::Model(std::string objectPath, std::string texturePath, MainWindow* mainWindow, Renderer* renderer) : RenderObject(mainWindow, renderer)
{
	this->objectPath = objectPath;
	this->texturePath = texturePath;
}

Model::~Model()
{
}
