#pragma once

#include <array>

#include "dengine/platform/CommonGL.h"
#include "GfxUtils.h"

namespace Dg
{
class BoundingBox
{
  public:
	glm::vec3 m_min{};
	glm::vec3 m_max{};

	BoundingBox() = default;
	BoundingBox(glm::vec3 min, glm::vec3 max) : m_min(min), m_max(max) {}

	std::array<glm::vec3, 8> getPoints()
	{
		std::array<glm::vec3, 8> arr{};
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				arr[i][j] = GfxUtils::g_ndcPoints[i][j] < 0 ? m_min[j] : m_max[j];
			}
		}
		return arr;
	}

	std::array<glm::vec3, 8> getTransformedPoints(glm::mat4 transform)
	{
		std::array<glm::vec3, 8> points = getPoints();
		std::array<glm::vec3, 8> transformedPoints{};
		for (int i = 0; i < 8; i++)
		{
			glm::vec4 transformedPoint = glm::vec4(points[i], 1.0f);
			transformedPoint = transform * transformedPoint;
			transformedPoints[i] = glm::vec3(transformedPoint);
		}
		return transformedPoints;
	}

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
		auto xExtremes =
		    std::minmax_element(pointsArr, end, [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.x < rhs.x; });
		auto yExtremes =
		    std::minmax_element(pointsArr, end, [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.y < rhs.y; });
		auto zExtremes =
		    std::minmax_element(pointsArr, end, [](const glm::vec3& lhs, const glm::vec3& rhs) { return lhs.z < rhs.z; });

		glm::vec3 min(xExtremes.first->x, yExtremes.first->y, zExtremes.first->z);
		glm::vec3 max(xExtremes.second->x, yExtremes.second->y, zExtremes.second->z);
		return {min, max};
	}
};

} // namespace Dg
