#pragma once

#include <iostream>

#include "dengine/platform/DGL.h"

#include "GLFW/glfw3.h"

#include "dengine/platform/Logger.h"
#include "dengine/resources/ResourceManager.h"

class Application;
using App = Application;

class Application {
  public:
	GLFWwindow* m_window;

	Application() = default;

	int run(int argc, char *argv[]);

	static Application* getInstance()
	{

	}
};