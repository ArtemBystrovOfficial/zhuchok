#pragma once

#include <memory>
#include <vector>

namespace common {

enum class CellType : char { Empty = ' ', Wall = 'W', Bug = 'B' };

class Cell {
public:
  Cell(char cell_type);
  ~Cell() = default;

  bool isWall();

private:
  CellType m_type = CellType::Empty;
};

class ConstMap {
public:
  ConstMap(const std::vector<std::vector<char>> &field);

  Cell getCell(int x, int y);

private:
  std::vector<std::vector<Cell>> m_field;
};

using const_map_ptr = std::unique_ptr<ConstMap>;

class MockMap : public ConstMap {
  using ConstMap::ConstMap;
};

} // namespace common
