#include "pch.h"
#include "WindowController.h"
#include "MainWindow.h"
#include "RenderObject.h"

WindowController::WindowController()
{
}


WindowController::~WindowController()
{
}

void WindowController::mouseBtnCallback(GLFWwindow * window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mousePressed = true;
		glfwGetCursorPos(window, &mouseX, &mouseY);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mousePressed = false;
	}
}

void WindowController::mouseMoveCallback(GLFWwindow * window, double xPos, double yPos) {
	MainWindow* mainWindow = &MainWindow::getInstance();

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

void WindowController::saveImageCombinationCallback(GLFWwindow * window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_CONTROL) {
		ctrlPressed = action == GLFW_PRESS;
		std::cout << "Ctrl pressed: " + ctrlPressed << std::endl;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS && ctrlPressed) {
		recording = !recording;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS && ctrlPressed) {
		shouldCreateVideo = !recording;
	}
}

void WindowController::setShouldSaveScreenshot(bool shouldSave)
{
	recording = shouldSave;
}

bool WindowController::shouldTakeScreenshot()
{
	return recording;
}

bool WindowController::getShouldCreateVideo()
{
	return shouldCreateVideo;
}

void WindowController::setShouldCreateVideo(bool should)
{
	shouldCreateVideo = should;
}
