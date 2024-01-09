
#pragma once

#include "Viewport/scene/Scene.h"

namespace Vp
{
class OrbitCamera;

class PreviewScene : public Scene
{
public:
	std::shared_ptr<OrbitCamera> m_orbitCamera;

	explicit PreviewScene();

	void init() override;
};
} // namespace Vp
