#include "PSSMInstancingShader.h"

namespace Dg
{
PSSMInstancingShader::PSSMInstancingShader(GLuint id) : ObjectShader(id)
{
	init(false);
}

void PSSMInstancingShader::init(bool initSuperclass)
{
	if (initSuperclass)
		ObjectShader::init(true);

	m_pvmOnly = true;

	cropMatricesLoc = glGetUniformLocation(m_id, "u_croppedMatrix");
	splitBeginLoc = glGetUniformLocation(m_id, "u_splitBegin");
	splitEndLoc = glGetUniformLocation(m_id, "u_splitEnd");
}

void PSSMInstancingShader::setUniforms()
{
	ObjectShader::setUniforms();

	glUniformMatrix4fv(cropMatricesLoc, cropMatrices.size(), false, (float*) cropMatrices.data());
	glUniform1i(splitBeginLoc, splitBegin);
	glUniform1i(splitEndLoc, splitEnd);
}
} // namespace Dg