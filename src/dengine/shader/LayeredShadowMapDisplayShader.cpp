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
	m_layersLoc = glGetUniformLocation(m_id, "u_layers");
}

void LayeredShadowMapDisplayShader::setUniforms()
{
	glUniform1i(m_layersLoc, m_layers);
	if (m_layers > 1)
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
