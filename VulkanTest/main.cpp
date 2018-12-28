#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "MainWindow.h"
#include "ColorChanger.h"
#include "Square.h"
#include "SquareDepth.h"
#include "Pipeline.h"
#include <array>
#include <chrono>

int mainMenu();
void renderModes(int, Renderer*, MainWindow*);

int main(int argn, char** argv)
{
	int mode = -1;

	while (mode != 0) {
		mode = mainMenu();

		std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();
		MainWindow* window = nullptr;

		renderer->createWindow(800, 600, "RikisWindow");

		window = renderer->getMainWindowPTR();

		renderer->continueInitialization();
		window->continueInitialization(renderer.get());

		renderModes(mode, renderer.get(), window);
	}
	
	return 0;
}

int mainMenu() {
	int option = -1;
	std::cout << "********* Write a number to choose what to render ********" << std::endl;
	std::cout << "1: Textured square" << std::endl;
	std::cout << "2: Depth buffered textured squares" << std::endl;
	//std::cout << "3: Background and depth buffers" << std::endl;
	std::cout << "0: Exit" << std::endl;
	std::cout << "Your choice: ";
	std::cin >> option;
	std::cout << "Good luck!!!!" << std::endl;
	std::cout << std::endl << std::endl << std::endl << std::endl;
	Util::setOption(option);

	return option;
}

void renderModes(int argn, Renderer* renderer, MainWindow* window) {
	switch (argn) {
		case 1: {
			Square square = Square(window, renderer);
			square.setName("Textured Square");
			square.render(window->getPipelinePTR()->getViewportPTR());
		}
		case 2: {
			SquareDepth depth = SquareDepth(window, renderer);
			depth.setName("Depth squares textured");
			depth.render(window->getPipelinePTR()->getViewportPTR());
		}
		/*case 3: {

		}*/
	}
}


/*void renderColors(Renderer* renderer, MainWindow* window) {
	ColorChanger colorChanger(window, renderer);

	colorChanger.render(window->getPipelinePTR()->getViewportPTR());
}*/