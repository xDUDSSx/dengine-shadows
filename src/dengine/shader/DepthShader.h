#pragma once

#include "ObjectShader.h"

namespace Dg
{
class DepthShader : public ObjectShader
{
  public:
	explicit DepthShader(GLuint id);

	void init(bool initSuperclass) override;
	void setUniforms() override;
};
} // namespace Dg
