#include "GameRunner.hpp"
#include <iostream>

int main() {
    common::MockMap mp(1,2);
    std::cout << game_runner::GameRunner(mp);
}