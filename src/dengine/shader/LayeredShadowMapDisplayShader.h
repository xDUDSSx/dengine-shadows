
#pragma once

#include "Shader.h"

namespace Dg
{
class LayeredShadowMapDisplayShader : public Shader
{
protected:
	GLint m_sourceSampler;
	GLint m_layeredSourceSampler;

	GLint m_resolutionId;
	GLint m_layeredLoc;
public:
	GLuint m_sourceTextureId{0};
	bool m_layered{false};

	glm::vec2 m_resolution{glm::vec2(1.0f)};

	explicit LayeredShadowMapDisplayShader(GLuint id);

	void init(bool initSuperclass) override;
	void setUniforms() override;
};
} // namespace Dg
