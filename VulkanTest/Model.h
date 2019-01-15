#pragma once
#include "PLATFORM.h"
#include "RenderObject.h"
#include "Texture.h"
#include <unordered_map>
#include <ppl.h>

class Model : public RenderObject
{
public:
	/*
		1.OBJ or GLTF path
		2.Texture path
	*/

	Model(std::string, MainWindow*, Renderer*);
	~Model();
private:
	void loadModel();
	//void parallelLoadingIsBadIdea();						WONT WORK! Messes up VERTICES/INDICES order
	std::string objectPath;
	
};

