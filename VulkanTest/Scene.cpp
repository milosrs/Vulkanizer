#include "pch.h"
#include "Scene.h"
#include "RenderObject.h"

Scene::Scene(std::vector<RenderObject*> renderObjects)
{
	this->renderObjects = renderObjects;
	descriptorHandler->createDescriptorSets(uniformBuffers, textures);
}


Scene::~Scene()
{
}
