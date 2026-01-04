#include "common.hpp"
#include <limits>

namespace common {

bool Cell::isWall() { return m_type == CellType::Wall; }
unsigned int Cell::GetSteps() {
    if (isWall()) {
        return std::numeric_limits<unsigned int>().max();
    }
    return m_steps;
}

} // namespace common
