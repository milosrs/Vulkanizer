#include "pch.h"
#include <iostream>
#include "vulkan\vulkan.h"
#include "Renderer.h"
#include "MainWindow.h"
#include "ColorChanger.h"
#include "Square.h"
#include "SquareDepth.h"
#include "Pipeline.h"
#include "Model.h"
#include <array>
#include <chrono>

const std::string texturesPath = "../Textures/";
const std::string objectsPath = "../objects/obj/";
const std::string meshesPath = "../objects/meshes/";

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
	std::cout << "3: 1.5milion vertices house" << std::endl;
	std::cout << "4: Keyboard" << std::endl;
	std::cout << "5: Earth" << std::endl;
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
			square.setTextureParams(texturesPath + "mifka.jpg", 4);
			square.render(window->getPipelinePTR()->getViewportPTR());
		}; break;
		case 2: {
			SquareDepth depth = SquareDepth(window, renderer);
			depth.setName("Depth squares textured");
			depth.setTextureParams(texturesPath + "mifka.jpg", 4);
			depth.render(window->getPipelinePTR()->getViewportPTR());
		}; break;
		case 3: {
			Model house = Model(objectsPath + "chalet.obj", meshesPath + "chalet.jpg", 4, window, renderer);
			house.setName("Chalet");
			house.render(window->getPipelinePTR()->getViewportPTR());
		}; break;
		case 4: {
			Model house = Model(objectsPath + "laptop.obj", meshesPath + "laptop.png", 4, window, renderer);
			house.setName("Truck");
			house.render(window->getPipelinePTR()->getViewportPTR());
		}; break;
		case 5: {
			Model earth = Model(objectsPath + "earth.obj", meshesPath + "earth.png", 4, window, renderer);
			earth.setName("Earth");
			earth.render(window->getPipelinePTR()->getViewportPTR());
		}; break;
	}
}