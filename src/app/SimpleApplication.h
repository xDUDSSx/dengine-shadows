#pragma once

#include "dengine/platform/Application.h"
#include "dengine/platform/CommonImGui.h"
#include "dengine/scene/SceneRenderTarget.h"
#include "app/scenes/MainScene.h"

class SimpleApplication : public Dengine::Application
{
  public:
	Ptr<MainScene> m_scene;
	Ptr<Dg::SceneRenderTarget> m_renderTarget;
	Ptr<Dg::SceneRenderTarget> m_secondRenderTarget;
	Dg::DisplayOptions m_displayOptions;
	Dg::DisplayOptions m_secondDisplayOptions;

	bool m_secondaryWindowHovered{false};
//	Dg::RenderOptions::ShadowType m_shadowType{Dg::RenderOptions::ShadowType::PSSM_INSTANCED};
	int m_shadowType{static_cast<int>(Dg::RenderOptions::ShadowType::PSSM_GEO)};
	bool m_limitFps{true};

	float m_mainCameraNear = 1.5f;
	float m_mainCameraFar = 600.0f;

	int m_shadowResolution = 2048;
	int m_shadowCascadeCount = 4;

	SimpleApplication();

  protected:
	bool onInit() override;
	void onDisplay() override;
	void onUpdate(float dt) override;
	void onExit() override;
	void resetTimers();
};
