#include "SimpleApplication.h"

#include "GLFW/glfw3.h"

#include "dengine/platform/CommonImGui.h"
#include "dengine/platform/input/InputManager.h"

#include "dengine/resources/Shaper.h"
#include "dengine/shader/Shaders.h"

SimpleApplication::SimpleApplication() : Application("Dengine") {}

bool SimpleApplication::onInit()
{
	// Load shaders
	Dg::Shaders::instance().load();

	// Load shapes
	Dg::Shaper::initDefaultShapes();

	// Preload some useful models
	RMI.mesh("Data/Models/camera.gltf");

	// Create manipulators
	//	m_manipulators = std::make_shared<Manipulators>(this);

	// Setup scenes
	m_scene = std::make_shared<MainScene>();
	m_scene->init();

	// Create render target
	Dg::RenderOptions renderOptions;
	renderOptions.multisample = true;
	renderOptions.selection = true;
	if (!m_renderTarget)
	{
		m_renderTarget = m_scene->createRenderTarget(renderOptions);
	}

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void SimpleApplication::onDisplay()
{
	ImGui::ShowDemoWindow();

	int width = m_windowSize.x;
	int height = m_windowSize.y;

	m_scene->draw(width, height, *m_renderTarget, m_displayOptions);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_renderTarget->getOutputFramebuffer().lock()->getId());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void SimpleApplication::onUpdate(float dt)
{
	int width = m_windowSize.x;
	int height = m_windowSize.y;
	m_scene->processInput(dt, InputManager::getMousePos(), m_windowSize);
}
