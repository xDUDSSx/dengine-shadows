#pragma once

#include <algorithm>
#include <vector>

#include "glm/glm.hpp"

namespace Dg
{
namespace GfxUtils
{

void extractZNearZFar(glm::mat4 projectionMatrix, float& zNear, float& zFar);

inline void perspectiveDivide(glm::vec4& vec)
{
	vec.x /= vec.w;
	vec.y /= vec.w;
	vec.z /= vec.w;
	vec.w = 1.0f;
}

} // namespace GfxUtils

} // namespace Dg
