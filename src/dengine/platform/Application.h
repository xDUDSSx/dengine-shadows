#pragma once

#include <iostream>

#include "dengine/platform/DGL.h"

#include "GLFW/glfw3.h"

#include "dengine/platform/Logger.h"
#include "dengine/resources/ResourceManager.h"

#define DENGINE_DEBUG 1

namespace Dengine
{

class Application;
using App = Application;

enum DebugLevel
{
	NOTICE = 0,
	LOW,
	MEDIUM,
	HIGH,
};

class Application
{
  public:
#if DENGINE_DEBUG
	bool m_debug{true};
#else
	bool m_debug{false};
#endif
	DebugLevel m_debugLevel{MEDIUM};

	std::string m_title;
	GLFWwindow* m_window{nullptr};
	glm::ivec2 m_windowSize{0};
	float m_UIScaleFactor{1.0f};

	explicit Application(const char* title);
	virtual ~Application() = default;

	int run(int argc, char* argv[]);

  private:
	int init(const char* title);
	void display();
	void update(float dt);
	void dispose();

  protected:
	virtual bool onInit() = 0;
	virtual void onDisplay() = 0;
	virtual void onUpdate(float dt) = 0;
	virtual void onExit() = 0;
};

// THIS IS A WORKAROUND TO MAKE IT EASY TO ACCESS THE CURRENT APPLICATION INSTANCE
// STATIC VARIABLES LIKE THIS SHOULD BE PHASED OUT IN THE FUTURE AND A DIFFERENT WAY TO ACCESS STATIC SYSTEMS LIKE INPUT /
// RESOURCE MANAGER SHOULD BE FOUND BUT RIGHT NOW USING THIS FOR SPEED OF DEVELOPMENT

static Application* g_activeApplication{nullptr};

inline Application& getApp()
{
	return *g_activeApplication;
}

static inline void setActiveApp(Application* app)
{
	g_activeApplication = app;
}

} // namespace Dengine