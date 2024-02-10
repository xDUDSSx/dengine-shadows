#pragma once

#include "ObjectShader.h"

namespace Dg
{
class ShadowShader : public ObjectShader
{
  protected:
	GLint m_zFarLoc;

	GLint m_lightPosLoc;

	GLint m_sunPosLoc;
	GLint m_sunDirLoc;

  public:
	float m_zFar{1.0f};
//	glm::vec3 m_lightPos{1.0f};

	glm::vec3 m_sunPos{1.0f};
	glm::vec3 m_sunDir{1.0f};

	explicit ShadowShader(GLuint id);

	void init(bool initSuperclass) override;
	void setUniforms() override;

	void getSunPositionFromViewMatrix(glm::mat4 view);
};
} // namespace Dg
