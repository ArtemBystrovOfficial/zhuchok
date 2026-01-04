#pragma once

#include "common.hpp"
#include "GameRunner.hpp"
#include "ThreadUtils.hpp"

#include <functional>
#include <iostream>

namespace map_generator {
    class MapGenerator {
        using callback_game_result = std::function<void(int, common::const_map_ptr)>;

        public:

            void generate(int h, int w) {
                thread_utils::MaxTracker tracker;

                while(true) {
                    auto mp = generateImpl(h, w);

                    if(!mp)
                        break;

                    callGameRunnerAsync(std::move(mp), [&tracker](int result, common::const_map_ptr mp) {
                        tracker.update_if_better(result, std::move(mp));
                    });
                }

                pool_.wait_until_task_taken();
                auto [best_score, best_map] = tracker.get_best();
                pool_.printStatistics();
                std::cout << "BEST SCORE:\n";
                std::cout << best_score << std::endl;
            }

        protected:
            virtual common::const_map_ptr generateImpl(int h, int w) = 0;

            void callGameRunnerAsync(common::const_map_ptr mp, callback_game_result cb) {
                struct Task {
                    common::const_map_ptr mp;
                    callback_game_result cb;
                    void operator()() {
                        auto result = game_runner::GameRunner(*mp);
                        cb(result, std::move(mp));
                    }
                };

                auto task = std::make_shared<Task>(Task{std::move(mp), std::move(cb)});

                pool_.enqueue([task]() {
                    (*task)();
                });
            }
        private:
            thread_utils::ThreadPool pool_;
    };

    class SomeGenerator : public MapGenerator {
        public:
            virtual common::const_map_ptr generateImpl(int h, int w) {
                if(!n--) {
                    return nullptr;
                }
                return std::make_unique<common::MockMap>(w, h);
            }
        private:
            int n = 100000;
    };
}