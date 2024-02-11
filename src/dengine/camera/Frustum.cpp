#include "Frustum.h"

#include "dengine/util/GfxUtils.h"
#include "dengine/util/BoundingBox.h"

namespace Dg
{

BoundingBox Frustum::createProjectedAABB(const glm::mat4& projection) const
{
	glm::vec3 transformedCorners[8];

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 transformedPoint = glm::vec4(m_corners[i], 1.0f);
		transformedPoint = projection * transformedPoint;
		GfxUtils::perspectiveDivide(transformedPoint);
		transformedCorners[i] = glm::vec3(transformedPoint);
	}
	return BoundingBox::createBoundingBox(transformedCorners, 8);
}

BoundingBox Frustum::createAABB() const
{
	return BoundingBox::createBoundingBox(m_corners, 8);
}

} // namespace Dg