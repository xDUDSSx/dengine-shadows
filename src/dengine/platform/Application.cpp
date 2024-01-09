#include "Application.h"

#include "dengine/platform/Logger.h"
#include "dengine/platform/input/InputManager.h"

void glfwErrorCallback(int error, const char* description)
{
	LOG_ERROR("Glfw error: {}", description);
}

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		InputManager::keyDown(key);
	}
	if (action == GLFW_RELEASE)
	{
		InputManager::keyUp(key);
	}
}

int Application::run(int argc, char** argv)
{
	Logger::init();

	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		return 1;
	}

	m_window = glfwCreateWindow(640, 480, "My Window", nullptr, nullptr);
	if (!m_window)
	{
		glfwTerminate();
		return 1;
	}

	glfwSetKeyCallback(m_window, glfwKeyCallback);
	glfwMakeContextCurrent(m_window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	while (!glfwWindowShouldClose(m_window))
	{
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}
