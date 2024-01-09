
#pragma once

#include "glm/vec3.hpp"

#include "HighlightOptions.h"

namespace Vp
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

	glm::vec3 clearColor;

	HighlightOptions highlight;

	RenderOptions();
};

} // namespace Vp
