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

	SimpleApplication();

  protected:
	bool onInit() override;
	void onDisplay() override;
	void onUpdate(float dt) override;
	void onExit() override;
};
