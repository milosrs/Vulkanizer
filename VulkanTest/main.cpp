#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "MainWindow.h"
#include "ColorChanger.h"
#include "Triangle.h"
#include "Pipeline.h"
#include <array>
#include <chrono>

void renderTriangle(Renderer*, MainWindow*);
void renderColors(Renderer*, MainWindow*);
void renderModes(int, Renderer*, MainWindow*);

int main(int argn, char** argv)
{
	std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
	MainWindow* window = nullptr;
	
	renderer->createWindow(800, 600, "RikisWindow");

	window = renderer->getMainWindowPTR();

	renderer->continueInitialization();
	window->continueInitialization(renderer.get());

	renderModes(argn, renderer.get(), window);

	return 0;
}

void renderModes(int argn, Renderer* renderer, MainWindow* window) {
	switch (argn) {
	case 1: renderColors(renderer, window);
	case 2: renderTriangle(renderer, window);
	}
}

void renderTriangle(Renderer* renderer, MainWindow* window) {
	Triangle triangle = Triangle(window, renderer);

	triangle.render(window->getPipelinePTR()->getViewportPTR());
}

void renderColors(Renderer* renderer, MainWindow* window) {
	ColorChanger colorChanger(window, renderer);

	colorChanger.render(window->getPipelinePTR()->getViewportPTR());
}

