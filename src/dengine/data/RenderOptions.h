
#pragma once

#include "glm/vec3.hpp"

#include "HighlightOptions.h"
#include "dengine/camera/Frustum.h"

namespace Dg
{
/**
 * Options specifying how a scene should be rendered.
 */
class RenderOptions
{
  public:
	enum class ShadowType
	{
		REGULAR = 0,
		PSSM_GEO = 1,
		PSSM_INSTANCED = 2,
	};

	bool multisample;
	unsigned int samples;
	bool framebufferAlpha;
	bool wboit;
	int wboitFunc;
	bool selection;
	int lightingModel;
	bool shadows;
	RenderOptions::ShadowType shadowType;
	int shadowCascadesCount;
	float pssmShadowsSplitSchemeWeight;
	int shadowResolution;
	float shadowBias;
	bool profilingEnabled;

	Frustum tightShadowFrustum; // TODO: Remove

	glm::vec3 clearColor;

	HighlightOptions highlight;

	RenderOptions();
};

} // namespace Dg
