#include "SimpleApplication.h"

#include "GLFW/glfw3.h"

#include "dengine/platform/CommonImGui.h"
#include "dengine/platform/input/InputManager.h"
#include "dengine/camera/OrbitCamera.h"

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
	renderOptions.multisample = false;
	renderOptions.selection = false;
	renderOptions.shadows = true;
	if (!m_renderTarget)
	{
		m_renderTarget = m_scene->createRenderTarget(renderOptions);
	}

	// Create second render target
	Dg::RenderOptions secondRenderOptions;
	secondRenderOptions.multisample = false;
	secondRenderOptions.selection = false;
	secondRenderOptions.shadows = false;
	if (!m_secondRenderTarget)
	{
		m_secondRenderTarget = m_scene->createRenderTarget(secondRenderOptions);
	}
	m_secondDisplayOptions.showDebug = true;

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

//	ImGui::Begin("Framebuffers", NULL, ImGuiWindowFlags_AlwaysAutoResize);
//	GLuint texture = m_renderTarget->getFramebuffer("shadows").lock()->getDepthAttachment()->m_id;
//	//	GLuint texture = m_renderTarget->getFramebuffer(5).lock()->getDepthAttachment()->m_id;
//	//	GLuint texture = m_renderTarget->getOutputFramebuffer().lock()->getColorTexture();
//	// the uv coordinates flips the picture, since it was upside down at first
//	ImGui::Image((void*) (intptr_t) texture, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
//	ImGui::End();

	int sWidth = m_windowSize.x / 2;
	int sHeight = m_windowSize.y / 2;

	ImGui::SetNextWindowSizeConstraints(ImVec2(100, 20), ImVec2(FLT_MAX, FLT_MAX));
	ImGui::Begin("Secondary view", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	if (!ImGui::IsWindowCollapsed())
	{
		m_scene->m_orbitCamera2->size(sWidth, sHeight);
		m_scene->m_orbitCamera2->update();
		m_scene->draw(sWidth, sHeight, m_scene->m_orbitCamera2->getView(), m_scene->m_orbitCamera2->getProjection(),
		              *m_secondRenderTarget, m_secondDisplayOptions);
		GLuint texture2 = m_secondRenderTarget->getOutputFramebuffer().lock()->getColorTexture();
		ImGui::Image((void*) (intptr_t) texture2, ImVec2(sWidth, sHeight), ImVec2(0, 1), ImVec2(1, 0));
		m_secondaryWindowHovered = ImGui::IsWindowHovered();
	}
	else
	{
		m_secondaryWindowHovered = false;
	}

	ImGui::End();

	ImGui::Begin("Info", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	glm::vec3 cameraPos = m_scene->m_orbitCamera->getPosition();
	ImGui::Text("Camera position: %f, %f, %f", cameraPos.x, cameraPos.y, cameraPos.z);
	ImGui::End();

	if (ImGui::SliderAngle("Sun spin", &m_scene->m_sunSpin))
	{
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), m_scene->m_sunSpin, glm::vec3(0, 1, 0));
		m_scene->m_lighting->m_shadowSunLight.pos = glm::vec3(rot * glm::vec4(35, 30, 10, 1.0f));
		m_scene->m_lighting->m_shadowSunLight.direction = glm::normalize(-m_scene->m_lighting->m_shadowSunLight.pos);
		m_scene->m_lighting->m_shadowSunLight.updateShadowVolume(50, 1.0f, 100.0f);
	}
}

void SimpleApplication::onUpdate(float dt)
{
	//	m_scene->processInput(dt, InputManager::getMousePos(), m_windowSize);

	if (m_secondaryWindowHovered)
	{
		m_scene->m_orbitCamera2->processInput(dt, InputManager::getMousePos(), m_windowSize);
	}
	else
	{
		m_scene->m_orbitCamera->processInput(dt, InputManager::getMousePos(), m_windowSize);
	}
}
