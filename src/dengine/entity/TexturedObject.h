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

	explicit TexturedObject(Core::Mesh* mesh, PhongShader* shader = Dg::Shaders::instance().m_phongShader.get());

	void render(Shader* shader, glm::mat4 view, glm::mat4 projection, bool silhouette) override;
};
} // namespace Dg
