#pragma once
#include "PLATFORM.h"
#include <vector>

class RenderObject;

class Scene
{
public:
	Scene(std::vector<RenderObject*>);
	~Scene();

private:
	std::vector<RenderObject*> renderObjects;
};

