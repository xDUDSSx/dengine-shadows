#pragma once

#include "dengine/platform/Common.h"
#include "dengine/platform/DGL.h"

#include "dengine/camera/AbstractCamera.h"
#include "dengine/framebuffer/Framebuffer.h"

#include "Viewport/data/DisplayOptions.h"
#include "Viewport/data/RenderOptions.h"
#include "Viewport/scene/Scene.h"
#include "Viewport/scene/SceneRenderTarget.h"
#include "Viewport/scene/scenes/MainScene.h"
#include "Viewport/shader/Shaders.h"

namespace Vp
{
class PreviewScene;

class SimpleViewport
{
  private:
	Ptr<PreviewScene> m_scene;

  public:
	SimpleViewport() = default;
	~SimpleViewport() = default;

	void init();
};
}

