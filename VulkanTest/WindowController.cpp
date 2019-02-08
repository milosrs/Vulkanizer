#include "pch.h"
#include "WindowController.h"
#include "MainWindow.h"

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
	else if (key == GLFW_KEY_S && action == GLFW_PRESS && ctrlPressed && !savingInProgress) {
		recording = !recording;

		while (recording) {
			savingInProgress = true;
			MainWindow* mainWindow = &MainWindow::getInstance();
			std::string pictureName = picturePath + "screenshot_";

			pictureName += std::to_string(filenames.size()) + ".ppm";
			mainWindow->getSwapchain()->saveScreenshot(pictureName);
			savingInProgress = false;
			filenames.push_back(pictureName);
		}

		if (!recording) {
			std::ofstream file(picturePath + "input.txt", std::ios::out);
			double duration = 0.0001;

			for (const auto &filename : filenames) {
				std::string line1 = "file \'" + filename + "\'";
				std::string line2 = "duration " + std::to_string(duration);
			}

			system("ffmpeg -f concat -i input.txt -vsync vfr -pix_fmt yuv420p output.mp4");
		}
	}
}
