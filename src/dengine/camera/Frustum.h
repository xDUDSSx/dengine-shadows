#pragma once

#include "dengine/platform/CommonGL.h"

namespace Dg
{

class BoundingBox;

class Frustum
{
  public:
	glm::vec3 m_corners[8] {};

	BoundingBox createProjectedAABB(const glm::mat4& projection) const;
	BoundingBox createAABB() const;
};

} // namespace Dg
