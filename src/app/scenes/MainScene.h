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
	std::shared_ptr<Dg::OrbitCamera> m_orbitCamera2;

	float m_sunSpin = 0.f;

	explicit MainScene();

	void init() override;
	void draw(int width, int height, Dg::SceneRenderTarget& renderTarget, const Dg::DisplayOptions& displayOptions) override;
	void draw(int width, int height, glm::mat4 view, glm::mat4 projection, Dg::SceneRenderTarget& renderTarget,
	          const Dg::DisplayOptions& displayOptions) override;
};
