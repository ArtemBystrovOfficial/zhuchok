#pragma once

#include "common.hpp"
#include "GameRunner.hpp"
#include <functional>
#include <iostream>

namespace map_generator {
    class MapGenerator {
        using callback_game_result = std::function<void(int, common::const_map_ptr)>;

        public:
            virtual void generate(int h, int w) = 0;

        protected:
            void callGameRunnerAsync(common::const_map_ptr mp, callback_game_result cb) {
                //TODO thread pool
                auto result = game_runner::GameRunner(*mp);
                cb(result, std::move(mp));
            }
    };

    class SomeGenerator : public MapGenerator {
        public:
            void generate(int h, int w) override {
                common::const_map_ptr mp;
                callGameRunnerAsync(std::move(mp), [](int result, auto mp) {
                    std::cout << "RESULT: " << result;
                });
            }
    };
}