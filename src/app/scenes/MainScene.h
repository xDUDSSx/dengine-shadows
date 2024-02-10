#pragma once

#include "dengine/scene/Scene.h"

namespace Dg
{
class OrbitCamera;
}

class MainScene : public Dg::Scene
{
  public:
	std::shared_ptr<Dg::OrbitCamera> m_orbitCamera;

	explicit MainScene();

	void init() override;
	void draw(int width, int height, Dg::SceneRenderTarget& renderTarget, const Dg::DisplayOptions& displayOptions) override;
};
