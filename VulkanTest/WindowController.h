#pragma once
#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H
#endif
#include "PLATFORM.h"
#include "RenderObject.h"
#include <iostream>
#include <vector>

class MainWindow;

static bool mousePressed;
static double mouseX;
static double mouseY;
static MainWindow *mainWindow;

class WindowController
{
public:
	WindowController(MainWindow*);
	~WindowController();

	
	static void mouseBtnCallback(GLFWwindow *window, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			mousePressed = true;
			glfwGetCursorPos(window, &mouseX, &mouseY);
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			mousePressed = false;
		}
	}

	static void mouseMoveCallback(GLFWwindow *window, double xPos, double yPos) {
		if (mousePressed) {
			double deltaX = xPos - mouseX;
			double deltaY = yPos - mouseY;

			glm::vec2 mouseDelta = { deltaX, deltaY };
			
			if (mainWindow != nullptr) {
				std::vector<RenderObject*> objects = mainWindow->getRenderObjects();
				
				for (RenderObject *ro : objects) {
					ro->rotate(mouseDelta);
				}
			}
		}
	}
};

