#pragma once
#include "Scene.h"

class Triangle : public Scene
{
public:
	Triangle(MainWindow*, Renderer*);

	void render(VkViewport*) override;
	void recordFrameBuffer(CommandBuffer*, MainWindow*) override;
private:
	


};

