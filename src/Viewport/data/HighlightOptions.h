#pragma once

#include "glm/glm.hpp"

class HighlightOptions
{
  public:
	HighlightOptions();

	float downscaleFactor;
	int kernelSize;
	float outlineCutoff;
	bool useDepth;
	float useDepth_darkenFactor;
	float useDepth_desaturateFactor;

	glm::vec3 selectionColor;
	glm::vec3 highlightColor;
};