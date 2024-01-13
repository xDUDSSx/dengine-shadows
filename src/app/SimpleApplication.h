#pragma once

#include "dengine/platform/Application.h"
#include "dengine/scene/SceneRenderTarget.h"
#include "app/scenes/MainScene.h"

class SimpleApplication : public Dengine::Application
{
  public:
	Ptr<MainScene> m_scene;
	Ptr<Dg::SceneRenderTarget> m_renderTarget;
	Dg::DisplayOptions m_displayOptions;

	SimpleApplication();

  protected:
	bool onInit() override;
	void onDisplay() override;
	void onUpdate(float dt) override;
};
