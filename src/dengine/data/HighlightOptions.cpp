#include "HighlightOptions.h"

HighlightOptions::HighlightOptions()
{
	downscaleFactor = 0.5f;
	kernelSize = 2;
	outlineCutoff = 0.2f;
	useDepth = true;
	useDepth_darkenFactor = 0.5f;
	useDepth_desaturateFactor = 0.4f;

	selectionColor = glm::vec3(0.949, 0.682, 0.18);
	highlightColor = glm::vec3(0.18, 0.784, 0.949);
}
