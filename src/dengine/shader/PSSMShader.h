#pragma once

#include "ObjectShader.h"

namespace Dg
{
class PSSMShader : public ObjectShader
{
  protected:
	GLint cropMatricesLoc;

	GLint splitBeginLoc;
	GLint splitEndLoc;

  public:
	std::vector<glm::mat4> cropMatrices;
	int splitBegin;
	int splitEnd;

	explicit PSSMShader(GLuint id);

	void init(bool initSuperclass) override;
	void setUniforms() override;
};
} // namespace Dg
