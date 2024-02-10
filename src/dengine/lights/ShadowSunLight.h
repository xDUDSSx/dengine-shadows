#pragma once

#include "SunLight.h"
#include "ShadowCastingLight.h"

namespace Dg
{
class ShadowSunLight : public SunLight, public ShadowCastingLight
{
  public:
	void setUniforms(const Dg::PhongShader& shader, int index) const override;
	void updateShadowVolume(float width, float zNear, float zFar);
};

} // namespace Dg
