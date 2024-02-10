#pragma once

#include "dengine/platform/CommonGL.h"

namespace Dg
{
class BoundingBox
{
  public:
	glm::vec3 m_min;
	glm::vec3 m_max;

	BoundingBox(glm::vec3 min, glm::vec3 max) : m_min(min), m_max(max) {}

	static inline BoundingBox createBoundingBox(std::vector<glm::vec3> points)
	{
		auto xExtremes = std::minmax_element(points.begin(), points.end(),
		                                     [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.x < rhs.x; });
		auto yExtremes = std::minmax_element(points.begin(), points.end(),
		                                     [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.y < rhs.y; });
		auto zExtremes = std::minmax_element(points.begin(), points.end(),
		                                     [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.z < rhs.z; });

		glm::vec3 min(xExtremes.first->x, yExtremes.first->y, zExtremes.first->z);
		glm::vec3 max(xExtremes.second->x, yExtremes.second->y, zExtremes.second->z);
		return {min, max};
	}

	static inline BoundingBox createBoundingBox(const glm::vec3* pointsArr, unsigned int size)
	{
		const glm::vec3* end = pointsArr + size;
		auto xExtremes = std::minmax_element(pointsArr, end,
		                                     [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.x < rhs.x; });
		auto yExtremes = std::minmax_element(pointsArr, end,
		                                     [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.y < rhs.y; });
		auto zExtremes = std::minmax_element(pointsArr, end,
		                                     [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.z < rhs.z; });

		glm::vec3 min(xExtremes.first->x, yExtremes.first->y, zExtremes.first->z);
		glm::vec3 max(xExtremes.second->x, yExtremes.second->y, zExtremes.second->z);
		return {min, max};
	}
};

} // namespace Dg
