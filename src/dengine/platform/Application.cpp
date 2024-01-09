#include "Application.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "dengine/platform/Logger.h"
#include "dengine/platform/input/InputManager.h"
#include "dengine/util/GLUtils.h"

void glfwErrorCallback(int error, const char* description)
{
	LOG_ERROR("GLFW error: {}", description);
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

void GLAPIENTRY openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                                    const void* userParam)
{
	LOG_ERROR("[OGL DEBUG] [{}] [{}, {}]: {}", GLUtils::Debug::severityToString(severity), GLUtils::Debug::sourceToString(source), GLUtils::Debug::typeToString(type), message)
}

int Application::run(int argc, char** argv)
{
	init();

	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(); // Show demo window! :)

		// Rendering
		display();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_window);
	}

	// Destroy imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Destroy glfw
	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}

int Application::init()
{
	Logger::init();

	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, m_debug ? GLFW_TRUE : GLFW_FALSE);

	m_window = glfwCreateWindow(640, 480, "Dengine", nullptr, nullptr);
	if (!m_window)
	{
		LOG_ERROR("Failed to create GLFW window!");
		glfwTerminate();
		return 1;
	}

	//	// Set icon
	//	int x, y, channels;
	//	constexpr int desiredChannels = 4;
	//	auto* pixels = stbi_load("Data/Textures/logoi3t.png", &x, &y, &channels, desiredChannels);
	//	if (pixels)
	//	{
	//		GLFWimage image{x, y, pixels};
	//		glfwSetWindowIcon(m_mainWindow, 1, &image);
	//	}
	//	stbi_image_free(pixels);

	glfwSetKeyCallback(m_window, glfwKeyCallback);
	glfwMakeContextCurrent(m_window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	LOG_INFO("VENDOR:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	LOG_INFO("RENDERER: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	LOG_INFO("VERSION:  {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	LOG_INFO("GLSL:     {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	// Enable OGL debug
	if (m_debug && glfwExtensionSupported("GL_ARB_debug_output"))
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openGlDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// Init GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	ImGui::GetStyle().ScaleAllSizes(m_UIScaleFactor);
	ImGui::GetIO().FontGlobalScale = m_UIScaleFactor;
}

void Application::display()
{
	glClearColor(0.65f, 0.81f, 0.60f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}