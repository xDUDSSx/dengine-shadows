
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
	bool multisample;
	unsigned int samples;
	bool framebufferAlpha;
	bool wboit;
	int wboitFunc;
	bool selection;
	int lightingModel;
	bool shadows;

	Frustum tightShadowFrustum;

	glm::vec3 clearColor;

	HighlightOptions highlight;

	RenderOptions();
};

} // namespace Dg
