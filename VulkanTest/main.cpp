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
#include "Util.h"
#include <array>
#include <chrono>

int mainMenu();
void renderModes(int);

int main(int argn, char** argv)
{
	int mode = -1;

	while (mode != 0) {
		mode = mainMenu();

		MainWindow* window = nullptr;
		window = &MainWindow::getInstance();
		window->continueInitialization();

		renderModes(mode);
	}
	
	return 0;
}

int mainMenu() {
	int option = -1;
	std::cout << "********* Write a number to choose what to render ********" << std::endl;
	std::cout << "1: Textured square" << std::endl;
	std::cout << "2: Depth buffered textured squares" << std::endl;
	std::cout << "3: 1.5milion vertices house" << std::endl;
	std::cout << "4: Keyboard (Texture problem)" << std::endl;
	std::cout << "5: Earth" << std::endl;
	std::cout << "6: Crypt" << std::endl;
	std::cout << "7: Tank" << std::endl;
	std::cout << "8: Animated Morph Cube - official glTF model" << std::endl;
	std::cout << "0: Exit" << std::endl;
	std::cout << "Your choice: ";
	std::cin >> option;
	std::cout << "Good luck!!!!" << std::endl;
	std::cout << std::endl << std::endl << std::endl << std::endl;
	Util::setOption(option);

	return option;
}

void renderModes(int argn) {
	switch (argn) {
		case 1: {
			Square square = Square();
			square.setName("Textured Square");
			square.setTextureParams({ texturesPath + "mifka.jpg" }, 4);
			square.render();
		}; break;
		case 2: {
			SquareDepth depth = SquareDepth();
			depth.setName("Depth squares textured");
			depth.setTextureParams({ texturesPath + "bubbles.jpg" }, 4);
			depth.render();
		}; break;
		case 3: {
			Model house = Model("chalet.obj");
			house.setName("Chalet");
			house.setTextureParams({ meshesPath + "chalet.jpg" }, 4);
			house.render();
		}; break;
		case 4: {
			Model laptop = Model("laptop.obj");
			laptop.setName("Laptop");
			laptop.setTextureParams({ meshesPath + "laptop.png" }, 4);
			laptop.render();
		}; break;
		case 5: {
			Model earth = Model("earth.obj");
			earth.setName("Earth");
			earth.setTextureParams({ meshesPath + "earth.png" }, 4);
			earth.render();
		}; break;
		case 6: {
			Model test = Model("crypt/crypt.gltf");
			test.setName("Test");
		}
		case 7: {
			Model test = Model("vegeTank/vegeTank.gltf");
			test.setName("Vege Tank");
		}
		case 8: {
			Model test = Model("animatedMorphCube/AnimatedMorphCube.gltf");
			test.setName("Morph Cube");
		}
	}
}