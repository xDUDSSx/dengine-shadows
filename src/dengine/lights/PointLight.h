#pragma once

#include "Light.h"

namespace Dg
{
class PointLight : public Light
{
public:
	float radius = 12.0f;
	void setUniforms(const PhongShader& shader, int index) const;
};
} // namespace Dg