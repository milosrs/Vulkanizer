#pragma once
#include "Scene.h"

class Triangle : public Scene
{
public:
	Triangle(MainWindow*, Renderer*, CommandBuffer*, CommandPool*);
	~Triangle();

	void render(VkViewport*) override;
	void recordFrameBuffer(CommandBuffer*, MainWindow*) override;
private:
	


};

