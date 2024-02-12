#include "PSSMShader.h"

namespace Dg
{
PSSMShader::PSSMShader(GLuint id) : ObjectShader(id)
{
	init(false);
}

void PSSMShader::init(bool initSuperclass)
{
	if (initSuperclass)
		ObjectShader::init(true);

	m_pvmOnly = true;

	cropMatricesLoc = glGetUniformLocation(m_id, "u_croppedMatrix");
	splitBeginLoc = glGetUniformLocation(m_id, "u_splitBegin");
	splitEndLoc = glGetUniformLocation(m_id, "u_splitEnd");
}

void PSSMShader::setUniforms()
{
	ObjectShader::setUniforms();

	glUniformMatrix4fv(cropMatricesLoc, cropMatrices.size(), false, (float*) cropMatrices.data());
	glUniform1i(splitBeginLoc, splitBegin);
	glUniform1i(splitEndLoc, splitEnd);
}
} // namespace Dg