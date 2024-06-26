
#pragma once

namespace Dg
{
class Entity;

/**
 * Various viewport display settings.
 */
class DisplayOptions
{
public:
	// Entity visibility options
	bool showDefault{true};
	bool showAxes{true};
	bool showGrid{true};
	bool showCamera{true};
	bool showFrustum{true};

	// Grid options
	bool showGridLines{true};
	bool showGridXAxis{true};
	bool showGridYAxis{false};
	bool showGridZAxis{true};

	bool showDebug{false};
	bool showDebugRenderReceiversOnly{false};
	bool showDebugFrustumAABBs{false};
	bool showDebugFrustums{true};
	bool showDebugShadowMapVolumes{true};
	bool debugVisualizeShadowMap{false};
	bool debugDrawShadowMap{false};

	DisplayOptions() = default;

	bool shouldDraw(const Entity& entity) const;
};
} // namespace Dg
