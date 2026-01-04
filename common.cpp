#include "common.hpp"

namespace common {

bool Cell::isWall() { return m_type == CellType::Wall; }

} // namespace common
