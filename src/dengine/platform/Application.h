#pragma once

#include <iostream>

#include "dengine/platform/DGL.h"

#include "GLFW/glfw3.h"

#include "dengine/platform/Logger.h"
#include "dengine/resources/ResourceManager.h"

#define DENGINE_DEBUG 1

class Application;
using App = Application;

class Application
{
  public:
	GLFWwindow* m_window{nullptr};
	float m_UIScaleFactor{1.0f};
#if DENGINE_DEBUG
	bool m_debug{true};
#else
	bool m_debug{false};
#endif

	Application() = default;

	int run(int argc, char* argv[]);

  private:
	int init();
	void display();

  public:
	static inline Application& getInstance()
	{
		static Application app;
		return app;
	}
};