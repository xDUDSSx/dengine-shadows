#pragma once

#include "Viewport/scene/Scene.h"

namespace Vp
{
class OrbitCamera;
}

class MainScene : public Vp::Scene
{
  public:
	std::shared_ptr<Vp::OrbitCamera> m_orbitCamera;

	explicit MainScene();

	void init() override;
};
