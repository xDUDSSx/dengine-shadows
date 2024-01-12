#pragma once

#include "dengine/platform/Application.h"
#include "Viewport/scene/SceneRenderTarget.h"
#include "app/scenes/MainScene.h"

class SimpleApplication : public Dengine::Application
{
  public:
	Ptr<MainScene> m_scene;
	Ptr<Vp::SceneRenderTarget> m_renderTarget;
	Vp::DisplayOptions m_displayOptions;

	SimpleApplication();

  protected:
	bool onInit() override;
	void onDisplay() override;
	void onUpdate(float dt) override;
};
