#pragma once

#include "GameObject.h"

#include "dengine/shader/Shaders.h"

namespace Dg
{
class PhongShader;

class TexturedObject : public GameObject
{
  public:
	glm::vec3 m_tint{1.0f};
	float m_tintStrength{1.0f};

	explicit TexturedObject(Dg::Mesh* mesh, PhongShader* shader = Dg::Shaders::instance().getShader<PhongShader>().get());

	void prepareRenderContext(Renderer::RenderContext& context) override;
};
} // namespace Dg
