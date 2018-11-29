#pragma once
#include "Scene.h"
#include "Vertices.h"

class Triangle : public Scene
{
public:
	Triangle(MainWindow*, Renderer*);

	void render(VkViewport*) override;
	void recordFrameBuffer(CommandBuffer*) override;
private:
	std::shared_ptr<Vertices> vertices = nullptr;
};

