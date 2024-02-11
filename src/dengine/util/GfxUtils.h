#pragma once

#include <algorithm>
#include <vector>

#include "glm/glm.hpp"

#include "dengine/camera/Frustum.h"

namespace Dg
{
namespace GfxUtils
{

static glm::vec3 g_ndcPoints[8] = {glm::vec3(-1, -1, -1), glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1),
                                 glm::vec3(-1, -1, 1),  glm::vec3(-1, 1, 1),  glm::vec3(1, 1, 1),  glm::vec3(1, -1, 1)};

void extractZNearZFar(glm::mat4 projectionMatrix, float& zNear, float& zFar);

inline void perspectiveDivide(glm::vec4& vec)
{
	vec.x /= vec.w;
	vec.y /= vec.w;
	vec.z /= vec.w;
	vec.w = 1.0f;
}

inline Frustum unprojectMatrix(glm::mat4 projection)
{
	glm::mat4 projectionInv = glm::inverse(projection);
	Frustum frustum;
	for (int i = 0; i < 8; i++)
	{
		glm::vec4 transformedPoint = projectionInv * glm::vec4(g_ndcPoints[i], 1.0f);
		GfxUtils::perspectiveDivide(transformedPoint);
		frustum.m_corners[i] = glm::vec3(transformedPoint);
	}
	return frustum;
}

} // namespace GfxUtils

} // namespace Dg
