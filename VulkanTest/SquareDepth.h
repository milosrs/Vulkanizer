#pragma once
#include "PLATFORM.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "Scene.h"

class SquareDepth : public Scene
{
public:
	SquareDepth(MainWindow*, Renderer*);
	~SquareDepth();
};

