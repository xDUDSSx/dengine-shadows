#include "DepthShader.h"

namespace Dg
{
DepthShader::DepthShader(GLuint id) : ObjectShader(id)
{
	init(false);
}

void DepthShader::init(bool initSuperclass)
{
	m_pvmOnly = true;
	ObjectShader::init(initSuperclass);
}

void DepthShader::setUniforms()
{
	ObjectShader::setUniforms();
}
} // namespace Dg