#include "BoundingBox.h"

namespace Dg
{

BoundingBox::BoundingBox() : m_min(std::numeric_limits<float>::max()), m_max(std::numeric_limits<float>::min()) {}

} // namespace Dg