#include "ShadowShader.h"

namespace Dg
{
ShadowShader::ShadowShader(GLuint id) : ObjectShader(id)
{
	init(false);
}

void ShadowShader::init(bool initSuperclass)
{
	if (initSuperclass)
		ObjectShader::init(true);

	m_zFarLoc = glGetUniformLocation(m_id, "u_zFar");

	m_sunDirLoc = glGetUniformLocation(m_id, "u_sunDir");
	m_sunPosLoc = glGetUniformLocation(m_id, "u_sunPos");
}

void ShadowShader::setUniforms()
{
	glUniform1f(m_zFarLoc, m_zFar);

	glUniform3fv(m_sunDirLoc, 1, glm::value_ptr(m_sunDir));
	glUniform3fv(m_sunPosLoc, 1, glm::value_ptr(m_sunPos));

	ObjectShader::setUniforms();
}

void ShadowShader::getSunPositionFromViewMatrix(glm::mat4 view)
{
	glm::mat4 viewInv = glm::inverse(view);
	m_sunDir = -viewInv[2];
	m_sunPos = viewInv[3];
}

} // namespace Dg