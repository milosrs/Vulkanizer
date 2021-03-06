#pragma once
#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H
#endif
#include "PLATFORM.h"
#include <iostream>
#include <vector>
#include <filesystem>

static bool mousePressed;
static double mouseX;
static double mouseY;
static double lastMouseX = 0;
static double lastMouseY = 0;

//Video data
static bool ctrlPressed;
static bool recording = false;
static bool shouldCreateVideo = false;

class MainWindow;
class RenderObject;

class WindowController
{
public:
	WindowController();
	~WindowController();

	static void mouseBtnCallback(GLFWwindow *window, int button, int action, int mods);
	static void mouseMoveCallback(GLFWwindow *window, double xPos, double yPos);
	static void saveImageCombinationCallback(GLFWwindow *, int, int, int, int);
	static void setShouldSaveScreenshot(bool shouldSave);
	static void keyboardArrowCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static bool shouldTakeScreenshot();
	static bool getShouldCreateVideo();
	static void setShouldCreateVideo(bool);
};

