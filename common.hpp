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
  Cell(char cell_type) {
    m_type = static_cast<CellType>(cell_type);
    visit_count = 0;
  }
  Cell(CellType cell_type) {
    m_type = cell_type;
    visit_count = 0;
  }
  Cell() = default;
  ~Cell() = default;

  CellType getCellType() const { return m_type; }
  void setCellType(CellType type) { m_type = type; }

  int getVisitCount() const { return visit_count; }
  void incrementVisitCount() { visit_count++; }
  void setVisitCount(int count) { visit_count = count; }

  bool isWall() const { return m_type == CellType::Wall; }

private:
  CellType m_type = CellType::Empty;
  int visit_count = 0; // Количество посещений ячейки
};


class BaseMap {
public:
  virtual Cell &cell(int x, int y) = 0;
  virtual const Cell &getCell(int x, int y) const = 0;
  virtual void print(bool print_map = false) const {};

  virtual unsigned int GetWidth() const = 0;
  virtual unsigned int GetHeight() const = 0;
};

using const_map_ptr = std::unique_ptr<BaseMap>;

class GenericMap : public BaseMap {
public:
  GenericMap(int h, int w): h_(h), w_(w), data_(h, std::vector<Cell>(w)) {};
  Cell &cell(int x, int y) override {
    return data_[y][x];
  }

  const Cell &getCell(int x, int y) const override {
    return data_[y][x];
  }

  void print(bool print_map) const override {
    std::cout << h_ << "x" << w_ << " ";
    if(print_map) {
      std::cout << std::endl;
      for(auto & row: data_) {
        for(auto & cell : row) {
            std::cout << static_cast<char>(cell.getCellType());
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
      for(auto & row: data_) {
        for(auto & cell : row) {
            std::cout << static_cast<int>(cell.getVisitCount()) << " ";
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
    }
  }

  unsigned int GetWidth() const override { return w_; }
  unsigned int GetHeight() const override { return h_; }

private:
  std::vector<std::vector<Cell>> data_;
  int h_, w_;
};

} // namespace common