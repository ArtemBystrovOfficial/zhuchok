#include "GameRunner.hpp"
#include "common.hpp"
#include <iostream>

int main() {
  auto mp = common::MockMap({{' ', 'W'}, {'W', ' '}});
  std::cout << game_runner::GameRunner(mp);
}
