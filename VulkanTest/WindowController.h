#pragma once
#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H
#endif
#include "PLATFORM.h"
#include "RenderObject.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>

static bool mousePressed;
static double mouseX;
static double mouseY;

//Video data
static bool ctrlPressed;
static bool savingInProgress;
static bool recording = false;
static const std::string picturePath = "../screnshotsForVideo/";
static std::vector<std::string> filenames;

class MainWindow;

class WindowController
{
public:
	WindowController();
	~WindowController();

	static void mouseBtnCallback(GLFWwindow *window, int button, int action, int mods);
	static void mouseMoveCallback(GLFWwindow *window, double xPos, double yPos);
	static void saveImageCombinationCallback(GLFWwindow *, int, int, int, int);
};

