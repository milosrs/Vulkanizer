#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#endif // !MAIN_WINDOW_H

#include "PLATFORM.h"
#include "BUILD_OPTIONS.h"
#include "Util.h"
#include <vector>
#include <string>
#include <assert.h>
#include <iostream>

class Renderer;

class MainWindow
{
public:
	MainWindow(Renderer* renderer, uint32_t sizeX, uint32_t sizeY, std::string windowName);
	~MainWindow();

	void close();
	bool update();

private:
	void InitOSWindow();
	void DeinitOSWindow();
	void UpdateOSWindow();
	void InitOSSurface();

	void InitSurface();
	void DestroySurface();

	Renderer* renderer = nullptr;
	VkSurfaceKHR surfaceKHR = nullptr;
	VkSurfaceCapabilitiesKHR surfaceCapatibilities = {};
	VkBool32 isWSISupported = false;
	VkSurfaceFormatKHR surfaceFormat = {};

	uint32_t sizeX = 512;
	uint32_t sizeY = 512;
	std::string name = "MainWindow";

	bool window_should_run = true;
	uint32_t							surfaceX = 512;
	uint32_t							surfaceY = 512;
	std::string							_window_name;

#if VK_USE_PLATFORM_WIN32_KHR
	HINSTANCE							win32_instance = NULL;
	HWND								win32_window = NULL;
	std::string							win32_class_name;
	static uint64_t						win32_class_id_counter;
#elif VK_USE_PLATFORM_XCB_KHR
	xcb_connection_t				*	xcb_connection = nullptr;
	xcb_screen_t					*	xcb_screen = nullptr;
	xcb_window_t						xcb_window = 0;
	xcb_intern_atom_reply_t			*	xcb_atom_window_reply = nullptr;
#endif
};

