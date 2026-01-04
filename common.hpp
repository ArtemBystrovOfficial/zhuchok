#pragma once

#include <memory>
#include <vector>
#include <iostream>

namespace common {

constexpr size_t MAX_MAP_WIDTH = 30;
constexpr size_t MAX_MAP_HEIGHT = 20;

enum class CellType : char { Empty = '-', Wall = 'W', Bug = 'B' };

class Cell {
public:
  Cell(char cell_type) { m_type = static_cast<CellType>(cell_type); }
  Cell(CellType cell_type) { m_type = cell_type; }
  Cell() = default;
  ~Cell() = default;

  CellType getCell() {return m_type;}

  bool isWall();

  // Количество посещений жука клеткой
  unsigned int GetSteps();

private:
  unsigned int m_steps = 0; // количество раз сколько
  CellType m_type = CellType::Empty;
};

// Базовый класс для определния карты
class BaseMap {
public:
  virtual Cell &cell(int x, int y) = 0;
  virtual const Cell &getCell(int x, int y) = 0;

  virtual unsigned int GetWidth() const = 0;
  virtual unsigned int GetHeight() const = 0;
};

using const_map_ptr = std::unique_ptr<BaseMap>;

class GenericMap : public BaseMap {
public:
  GenericMap(int h, int w): h_(h), w_(w), data_(h, std::vector<Cell>(w)) {
    cell(0,0) = Cell(CellType::Bug);
  };
  Cell &cell(int x, int y) override {
    return data_[y][x];
  }

  const Cell &getCell(int x, int y) override {
    return data_[y][x];
  }

  void print() {
    for(auto & row: data_) {
      for(auto & cell : row) {
          std::cout << static_cast<char>(cell.getCell());
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }

  unsigned int GetWidth() const override { return w_; }
  unsigned int GetHeight() const override { return h_; }

private:
  std::vector<std::vector<Cell>> data_;
  int h_, w_;
};

} // namespace common
