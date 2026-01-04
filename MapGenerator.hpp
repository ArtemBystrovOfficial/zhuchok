#pragma once

#include "common.hpp"
#include "GameRunner.hpp"
#include "ThreadUtils.hpp"

#include <functional>
#include <iostream>
#include <bitset>
#include <bit>
#include <cmath>

namespace map_generator {
    class MapGenerator {
        using callback_game_result = std::function<void(int, common::const_map_ptr)>;

        public:
            MapGenerator(int w, int h): h_(h), w_(w) {};

            void generate() {
                thread_utils::MaxTracker tracker;

                while(true) {
                    auto mp = generateImpl();

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
            virtual common::const_map_ptr generateImpl() = 0;

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

            int h_, w_;
        private:
            thread_utils::ThreadPool pool_;
    };

    class SomeGenerator : public MapGenerator {
        public:
            SomeGenerator(int h, int w) : MapGenerator(w, h) {};

            virtual common::const_map_ptr generateImpl() {
                if(!n--) {
                    return nullptr;
                }
                return std::make_unique<common::GenericMap>(h_, w_);
            }
        private:
            int n = 100000;
    };

    class CombinationsGenerator : public MapGenerator {
        public:
            //x-- x--
            //--- #--
            //--- ---
            CombinationsGenerator(int h, int w) : MapGenerator(w, h) {};

            virtual common::const_map_ptr generateImpl() {
                if(++cur_comb == max_comb)
                    return nullptr;

                auto mp = std::make_unique<common::GenericMap>(h_, w_);
                std::cout << 1 <<" " << cur_comb << std::endl;
                std::bitset<std::bit_width(common::MAX_MAP_WIDTH * common::MAX_MAP_HEIGHT-2)-1> bt(cur_comb<<1);
                std::cout << bt <<std::endl;
                for(int i=1; i < bt.size(); i++) {
                    int x = i%w_;
                    int y = i/h_;
                    std::cout <<"cord :" << x << " " << y << std::endl;
                    mp->cell(x,y) = common::Cell(common::CellType::Wall);
                }

                return mp;
            }
        private:
            size_t cur_comb = 0;
            size_t max_comb = std::pow(2, h_*w_-2);
    };
}