#pragma once

#include "BoundingBox.h"

namespace Dg
{
namespace Collisions
{

inline bool intersect(BoundingBox a, BoundingBox b)
{
	if (a.m_max.x < b.m_min.x || a.m_min.x > b.m_max.x)
		return false;
	if (a.m_max.y < b.m_min.y || a.m_min.y > b.m_max.y)
		return false;
	if (a.m_max.z < b.m_min.z || a.m_min.z > b.m_max.z)
		return false;
	return true;
}

namespace
{
inline std::pair<float, float> intersectSweepGetMinMaxProjection(const BoundingBox& box, const glm::vec3& sweepDir)
{
	// calculate projections along sweep direction

	// project AABB center point
	glm::vec3 vFrustumCenter = (box.m_min + box.m_max) * 0.5f;
	glm::vec3 vFrustumHalfSize = (box.m_max - box.m_min) * 0.5f;
	float fFrustumCenterProj = glm::dot(vFrustumCenter, sweepDir);

	// project AABB half-size
	float fFrustumHalfSizeProj =
	    vFrustumHalfSize.x * fabs(sweepDir.x) + vFrustumHalfSize.y * fabs(sweepDir.y) + vFrustumHalfSize.z * fabs(sweepDir.z);
	return {fFrustumCenterProj - fFrustumHalfSizeProj, fFrustumCenterProj + fFrustumHalfSizeProj};
}
} // namespace

/// AABB sweep intersection test
/// Mostly "borrowed" from the GPU Gems 3 Chapter 10 Demo code
inline bool intersectSweep(const BoundingBox& box, const BoundingBox& staticBox, const glm::vec3& sweepDir)
{
	// calculate projections along sweep direction

	// project AABB center point
	auto boxProjMinMax = intersectSweepGetMinMaxProjection(box, sweepDir);
	auto staticBoxProjMinMax = intersectSweepGetMinMaxProjection(staticBox, sweepDir);

	// find the distance in sweep direction
	// where intersection occurs on all axis.
	//

	// sweep direction intersection
	// starts: fObjectProjMax + fDist = fFrustumProjMin
	//   ends: fObjectProjMin + fDist = fFrustumProjMax
	float fDistMin = staticBoxProjMinMax.first - boxProjMinMax.second;
	float fDistMax = staticBoxProjMinMax.second - boxProjMinMax.first;
	if (fDistMin > fDistMax)
		std::swap(fDistMin, fDistMax);

	// only intersects in opposite of sweep direction
	if (fDistMax < 0)
		return false;

	// intersection on an axis:
	// starts: box.m_max.x + fDist*sweepDir.x = staticBox.m_min.x
	//   ends: box.m_min.x + fDist*sweepDir.x = staticBox.m_max.x

	// test x-axis:
	if (sweepDir.x == 0)
	{
		// there is never an intersection on this axis
		if (staticBox.m_min.x > box.m_max.x || box.m_min.x > staticBox.m_max.x)
			return false;
	}
	else
	{
		float fDistMinNew = (staticBox.m_min.x - box.m_max.x) / sweepDir.x;
		float fDistMaxNew = (staticBox.m_max.x - box.m_min.x) / sweepDir.x;
		if (fDistMinNew > fDistMaxNew)
			std::swap(fDistMinNew, fDistMaxNew);

		// distance ranges don't overlap
		if (fDistMin > fDistMaxNew || fDistMinNew > fDistMax)
			return false;
		// otherwise merge ranges
		fDistMin = std::max(fDistMin, fDistMinNew);
		fDistMax = std::min(fDistMax, fDistMaxNew);
	}

	// test y-axis:
	if (sweepDir.y == 0)
	{
		// there is never an intersection on this axis
		if (staticBox.m_min.y > box.m_max.y || box.m_min.y > staticBox.m_max.y)
			return false;
	}
	else
	{
		float fDistMinNew = (staticBox.m_min.y - box.m_max.y) / sweepDir.y;
		float fDistMaxNew = (staticBox.m_max.y - box.m_min.y) / sweepDir.y;
		if (fDistMinNew > fDistMaxNew)
			std::swap(fDistMinNew, fDistMaxNew);

		// distance ranges don't overlap
		if (fDistMin > fDistMaxNew || fDistMinNew > fDistMax)
			return false;
		// otherwise merge ranges
		fDistMin = std::max(fDistMin, fDistMinNew);
		fDistMax = std::min(fDistMax, fDistMaxNew);
	}

	// test z-axis:
	if (sweepDir.z == 0)
	{
		// there is never an intersection on this axis
		if (staticBox.m_min.z > box.m_max.z || box.m_min.z > staticBox.m_max.z)
			return false;
	}
	else
	{
		float fDistMinNew = (staticBox.m_min.z - box.m_max.z) / sweepDir.z;
		float fDistMaxNew = (staticBox.m_max.z - box.m_min.z) / sweepDir.z;
		if (fDistMinNew > fDistMaxNew)
			std::swap(fDistMinNew, fDistMaxNew);

		// distance ranges don't overlap
		if (fDistMin > fDistMaxNew || fDistMinNew > fDistMax)
			return false;
	}

	// all tests passed - intersection occurs
	return true;
}

} // namespace Collisions
} // namespace Dg
