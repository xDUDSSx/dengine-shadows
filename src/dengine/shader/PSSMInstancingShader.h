#pragma once

#include "ObjectShader.h"

namespace Dg
{
class PSSMInstancingShader : public ObjectShader
{
  protected:
	GLint cropMatricesLoc;

	GLint splitBeginLoc;
	GLint splitEndLoc;

  public:
	std::vector<glm::mat4> cropMatrices;
	int splitBegin;
	int splitEnd;

	explicit PSSMInstancingShader(GLuint id);

	void init(bool initSuperclass) override;
	void setUniforms() override;
};
} // namespace Dg
