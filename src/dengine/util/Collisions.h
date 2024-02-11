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

} // namespace Collisions
} // namespace Dg
