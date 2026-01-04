#include "GameRunner.hpp"
#include "common.hpp"
#include <atomic>

namespace game_runner {
int GameRunner(const common::BaseMap &mp) {
  static std::atomic_int count = 0;
  return count++; // TODO
}

Direction GetDirection(common::GenericMap &gm, int bug_x, int bug_y) {
  auto down_cell_steps = gm.cell(bug_x, bug_y + 1).GetSteps();
  auto right_cell_steps = gm.cell(bug_x + 1, bug_y).GetSteps();
  auto top_cell_steps = gm.cell(bug_x, bug_y - 1).GetSteps();
  auto left_cell_steps = gm.cell(bug_x - 1, bug_y).GetSteps();

  if (down_cell_steps <= right_cell_steps &&
      down_cell_steps <= top_cell_steps && down_cell_steps <= left_cell_steps) {
    return Direction::DOWN;
  }

  if (right_cell_steps <= top_cell_steps &&
      right_cell_steps <= left_cell_steps) {
    return Direction::RIGHT;
  }

  if (top_cell_steps <= left_cell_steps) {
    return Direction::UP;
  }

  return Direction::LEFT;
}
} // namespace game_runner
