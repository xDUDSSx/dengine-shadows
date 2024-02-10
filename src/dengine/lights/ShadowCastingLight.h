#pragma once

#include "dengine/lights/ShadowMap.h"

namespace Dg
{
class ShadowCastingLight
{
  public:
	Ptr<ShadowMap> m_shadowMap;

	virtual ~ShadowCastingLight() = default;
};

} // namespace Dg
