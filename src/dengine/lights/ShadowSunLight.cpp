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

	// TODO: Turn into a UBO probably, avoid calling glGetUniform all the time
	// TODO: Replace with cropped matrix
	//	glm::mat4 lightPvm = m_shadowMap->m_lightProjection * m_shadowMap->m_lightView;
	//	glm::mat4 lightPvmCropped = m_shadowMap->m_croppedLightProjection * m_shadowMap->m_lightView;
	glm::mat4 lightPvmCropped = m_shadowMap->m_cropMatrices[0] * m_shadowMap->m_lightProjection * m_shadowMap->m_lightView;
	// TODO: Remove
	glUniformMatrix4fv(glGetUniformLocation(shader.m_id, (prefix + ".lightPvm").c_str()), 1, GL_FALSE,
	                   glm::value_ptr(lightPvmCropped));

	int splits = m_shadowMap->m_splitCount;
	// Set the combined cropped light PVM matrices for each cascade
	glUniformMatrix4fv(glGetUniformLocation(shader.m_id, (prefix + ".lightPvms").c_str()), splits, GL_FALSE,
	                   (float*) m_shadowMap->m_lightPvmMatrices.data());
	// Set the float array of split plane camera view space distances
	glUniform1fv(glGetUniformLocation(shader.m_id, (prefix + ".splitPlanes").c_str()), splits, &m_shadowMap->m_splitPositions[1]);
}

void ShadowSunLight::updateShadowVolume(float width, float zNear, float zFar)
{
	if (!m_shadowMap)
		m_shadowMap = std::make_shared<ShadowMap>();
	m_shadowMap->setupShadowVolume(pos, pos + direction, width, zNear, zFar);
}

} // namespace Dg