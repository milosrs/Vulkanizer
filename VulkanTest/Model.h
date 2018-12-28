#pragma once
#include "PLATFORM.h"
#include "RenderObject.h"
#include "Texture.h"

class Model : public RenderObject
{
public:
	/*
		1.OBJ or GLTF path
		2.Texture path
	*/

	Model(std::string, std::string, MainWindow*, Renderer*);
	~Model();
private:
	std::string objectPath, texturePath;
};

