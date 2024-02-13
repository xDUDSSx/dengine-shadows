#include "LayeredShadowMapDisplayShader.h"

using namespace Dg;

LayeredShadowMapDisplayShader::LayeredShadowMapDisplayShader(GLuint id) : Shader(id)
{
	init(false);
}

void LayeredShadowMapDisplayShader::init(bool initSuperclass)
{
	if (initSuperclass)
		Shader::init(true);

	m_sourceSampler = glGetUniformLocation(m_id, "source");
	m_layeredSourceSampler = glGetUniformLocation(m_id, "sourceLayered");
	m_resolutionId = glGetUniformLocation(m_id, "u_resolution");
	m_layeredLoc = glGetUniformLocation(m_id, "u_layered");
}

void LayeredShadowMapDisplayShader::setUniforms()
{
	glUniform1i(m_layeredLoc, m_layered);
	if (m_layered)
	{
		bindTexture2DArray(5, m_sourceTextureId, m_layeredSourceSampler);
	}
	else
	{
		bindTexture2D(5, m_sourceTextureId, m_sourceSampler);
	}

	glUniform2fv(m_resolutionId, 1, glm::value_ptr(m_resolution));

	Shader::setUniforms();
}
