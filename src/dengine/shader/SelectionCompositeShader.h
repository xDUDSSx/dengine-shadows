
#pragma once

#include "Shader.h"

namespace Dg
{
class SelectionCompositeShader : public Shader
{
protected:
	GLint m_sourceSampler;

	GLint m_resolutionId;
	GLint m_cutoffId;

public:
	GLuint m_sourceTextureId{0};

	glm::vec2 m_resolution{glm::vec2(1.0f)};
	float m_cutoff{0.2f};

	explicit SelectionCompositeShader(GLuint id);

	void init(bool initSuperclass) override;
	void setUniforms() override;
};
} // namespace Dg
