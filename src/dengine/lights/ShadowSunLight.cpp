#include "ShadowSunLight.h"

#include "dengine/shader/PhongShader.h"
#include "dengine/lights/ShadowMap.h"

namespace Dg
{

void ShadowSunLight::setUniforms(const PhongShader& shader, int index) const
{
	const std::string prefix = "u_shadowSunLight";
	glUniform1f(glGetUniformLocation(shader.m_id, (prefix + ".intensity").c_str()), intensity);
	glUniform3fv(glGetUniformLocation(shader.m_id, (prefix + ".color").c_str()), 1, glm::value_ptr(color));
	glUniform3fv(glGetUniformLocation(shader.m_id, (prefix + ".specular").c_str()), 1, glm::value_ptr(specular));
	glUniform3fv(glGetUniformLocation(shader.m_id, (prefix + ".direction").c_str()), 1, glm::value_ptr(direction));

	// TODO: Replace with cropped matrix
//	glm::mat4 lightPvm = m_shadowMap->m_lightProjection * m_shadowMap->m_lightView;
//	glm::mat4 lightPvmCropped = m_shadowMap->m_croppedLightProjection * m_shadowMap->m_lightView;
	glm::mat4 lightPvmCropped = m_shadowMap->m_cropMatrices[0] * m_shadowMap->m_lightProjection * m_shadowMap->m_lightView;
	glUniformMatrix4fv(glGetUniformLocation(shader.m_id, (prefix + ".lightPvm").c_str()), 1, GL_FALSE, glm::value_ptr(lightPvmCropped));
}

void ShadowSunLight::updateShadowVolume(float width, float zNear, float zFar)
{
	if (!m_shadowMap)
		m_shadowMap = std::make_shared<ShadowMap>();
	m_shadowMap->setupShadowVolume(pos, pos + direction, width, zNear, zFar);
}

} // namespace Dg