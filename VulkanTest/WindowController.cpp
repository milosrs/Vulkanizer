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
	glm::vec2 mouseDelta = glm::vec2(0.0f, 0.0f);
	glm::vec3 axis = glm::vec3(0.0f, 0.0f, 0.0f);

	if (mousePressed) {
		double deltaX = xPos - mouseX;
		double deltaY = yPos - mouseY;
		double XdeltaDifference = glm::abs(lastMouseX) - glm::abs(deltaX);
		double YdeltaDifference = glm::abs(lastMouseY) - glm::abs(deltaY);

		double rotateX = XdeltaDifference == 0 ? 0.0f : 1.0f,
			rotateY = YdeltaDifference == 0 ? 0.0f : 1.0f;

		if (deltaX < lastMouseX) {
			rotateX *= -1;
		}
		else if (deltaY < lastMouseY) {
			rotateY *= -1;
		}

		mouseDelta = { deltaX, deltaY };
		axis = glm::normalize(glm::vec3(0.0f, rotateY, rotateX));

		if (mainWindow != nullptr) {
			std::vector<RenderObject*> objects = mainWindow->getRenderObjects();

			for (RenderObject *ro : objects) {
				ro->rotate(mouseDelta, axis);
			}
		}

		lastMouseX = deltaX;
		lastMouseY = deltaY;
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

void WindowController::keyboardArrowCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	MainWindow* mainWindow = &MainWindow::getInstance();
	glm::vec2 mouseDelta = glm::vec2(0.0f, 0.0f);
	glm::vec3 axis = glm::vec3(0.0f, 0.0f, 0.0f);

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
		mouseDelta = glm::vec2(0.0f, 1.0f);
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		axis = glm::vec3(0.0f, 1.0f, 0.0f);
		mouseDelta = glm::vec2(0.0f, -1.0f);
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		axis = glm::vec3(0.0f, 0.0f, 1.0f);
		mouseDelta = glm::vec2(-1.0f, 0.0f);
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		axis = glm::vec3(0.0f, 0.0f, 1.0f);
		mouseDelta = glm::vec2(1.0f, 0.0f);
	}

	if (mainWindow != nullptr) {
		std::vector<RenderObject*> objects = mainWindow->getRenderObjects();

		for (RenderObject *ro : objects) {
			ro->rotate(mouseDelta, axis);
		}
	}
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
