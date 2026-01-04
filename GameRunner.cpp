#include "GameRunner.hpp"
#include <atomic>
namespace game_runner {

    int GameRunner(const common::ConstMap & mp) {
        static std::atomic_int count = 0;
        return count++; //TODO
    }
}