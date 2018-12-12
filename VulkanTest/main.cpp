#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "Util.h"
#include "MainWindow.h"
#include "ColorChanger.h"
#include "Triangle.h"
#include "Pipeline.h"
#include <array>
#include <chrono>

void renderTriangle(Util*, Renderer*, MainWindow*);
void renderColors(Util*, Renderer*, MainWindow*);
void renderModes(int, Util*, Renderer*, MainWindow*);

int main(int argn, char** argv)
{
	Util* util = &Util::instance();
	std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
	MainWindow* window = nullptr;
	
	renderer->createWindow(800, 600, "RikisWindow");

	window = renderer->getMainWindowPTR();

	renderer->continueInitialization();
	window->continueInitialization(renderer.get());

	renderModes(argn, util, renderer.get(), window);

	return 0;
}

void renderModes(int argn, Util* util, Renderer* renderer, MainWindow* window) {
	switch (argn) {
	case 1: renderColors(util, renderer, window);
	case 2: renderTriangle(util, renderer, window);
	}
}

void renderTriangle(Util* util, Renderer* renderer, MainWindow* window) {
	Triangle triangle = Triangle(window, renderer);

	triangle.render(window->getPipelinePTR()->getViewportPTR());
}

void renderColors(Util* util, Renderer* renderer, MainWindow* window) {
	ColorChanger colorChanger(window, renderer);

	colorChanger.render(window->getPipelinePTR()->getViewportPTR());
}

