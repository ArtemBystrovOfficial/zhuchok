#pragma once

#include "common.hpp"

namespace game_runner {

// Направления в которые ходит жучок по приоритетам
enum class Direction {
  DOWN,
  RIGHT,
  UP,
  LEFT,
};

// Keep it simple
// Interface only for optimizite simulation of bug maze
int GameRunner(const common::BaseMap &mp);

// Получает направление в котором пойдет жук в следующем ходе
// Жук движется по приоритету вниз, вправо, вверх, влево
Direction GetDirection(common::GenericMap &gm, unsigned int bug_x, unsigned int bug_y);
} // namespace game_runner
