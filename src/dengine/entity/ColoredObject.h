#pragma once

#include "GameObject.h"

namespace Dg
{
class ColorShader;

class ColoredObject : public GameObject
{
public:
	bool m_useSingleColor{false};
	glm::vec3 m_singleColor{1.0f, 0.0f, 1.0f};

	ColoredObject(Dg::Mesh* mesh, ColorShader* shader);

	void prepareRenderContext(Renderer::RenderContext& context) override;

	const glm::vec3& getColor() const;
	void setColor(const glm::vec3* color);
	void setColor(const glm::vec3& color);
};
} // namespace Dg
