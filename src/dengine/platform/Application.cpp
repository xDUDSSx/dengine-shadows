#include "Application.h"

#include "dengine/platform/CommonImGui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "dengine/platform/Logger.h"
#include "dengine/platform/input/InputManager.h"
#include "dengine/util/GLUtils.h"

namespace Dengine
{

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
                                    const void* userParam){
    LOG_ERROR("[OGL DEBUG] [{}] [{}, {}]: {}", GLUtils::Debug::severityToString(severity), GLUtils::Debug::sourceToString(source),
              GLUtils::Debug::typeToString(type), message)}

Application::Application(const char* title)
    : m_title(title)
{
}

int Application::run(int argc, char** argv)
{
	init(m_title.c_str());

	while (!glfwWindowShouldClose(m_window))
	{
		glfwGetFramebufferSize(m_window, &m_windowSize.x, &m_windowSize.y);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		InputManager::beginFrame();

		// Logic update
		update(ImGui::GetIO().DeltaTime);

		// Rendering
		display();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		InputManager::endFrame();

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

int Application::init(const char* title)
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

	m_window = glfwCreateWindow(640, 480, title, nullptr, nullptr);
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

	LOG_INFO("==== OpenGL details ====");
	LOG_INFO("VENDOR:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	LOG_INFO("RENDERER: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	LOG_INFO("VERSION:  {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	LOG_INFO("GLSL:     {}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	// Enable OGL debug
	if (m_debug && glfwExtensionSupported("GL_ARB_debug_output"))
	{
		LOG_INFO("DEBUG:    Yes")
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openGlDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else
	{
		LOG_INFO("DEBUG:    No")
	}

	LOG_INFO("========================");

	// Init GUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	ImGui::GetStyle().ScaleAllSizes(m_UIScaleFactor);
	ImGui::GetIO().FontGlobalScale = m_UIScaleFactor;

	// Setup basic keybinds
	InputManager::bindKey("scroll", Keys::mouseScrlUp, {}, 1.0f);
	InputManager::bindKey("scroll", Keys::mouseScrlDown, {}, -1.0f);
	InputManager::bindKey("pan", Keys::mouseMiddle);
	InputManager::bindKey("rotate", Keys::mouseRight);

	onInit();
}

void Application::display()
{
	onDisplay();
}

void Application::update(float dt)
{
	onUpdate(dt);
}

} // namespace Dengine