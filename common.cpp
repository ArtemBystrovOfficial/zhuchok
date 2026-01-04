#include "common.hpp"
#include <vector>

namespace common {

Cell::Cell(char cell_type) : m_type(static_cast<CellType>(cell_type)) {};
bool Cell::isWall() { return m_type == CellType::Wall; }

FromTextMap::FromTextMap(const std::vector<std::vector<char>> &field) {
  m_field.reserve(field.size());

  const auto row_size = field[0].size();
  for (auto i = 0; i < field.size(); ++i) {
    m_field[i].reserve(row_size);

    auto last_ptr = &m_field.emplace_back();
    for (auto &char_ : field[i]) {
      last_ptr->push_back(Cell(char_));
    }
  }
}

Cell& FromTextMap::cell(int x, int y) { return m_field[y][x]; }

} // namespace common
