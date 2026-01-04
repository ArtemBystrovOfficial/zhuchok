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

                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                pool_.wait_until_task_taken();
                auto [best_score, best_map] = tracker.get_best();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                pool_.printStatistics();
                std::cout << "BEST SCORE:\n";
                std::cout << best_score << std::endl;
                std::cout << "BEST MAP:\n";
                best_map->print();
            }

        protected:

            //First important check
            bool isPathExists(const common::GenericMap& grid) {
                auto wall = common::CellType::Wall;
                auto empty = common::CellType::Empty;

                int h = grid.GetHeight();
                int w = grid.GetWidth();

                if (grid.getCell(0,0).getCellType() == wall || grid.getCell(w-1,h-1).getCellType() == wall) {
                    return false;
                }

                std::vector<std::vector<bool>> visited(h, std::vector<bool>(w, false));
                std::queue<std::pair<int, int>> q;

                q.push({0, 0});
                visited[0][0] = true;

                const int dirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
                while (!q.empty()) {
                    auto [y, x] = q.front();
                    q.pop();


                    if (y == h-1 && x == w-1) {
                        return true;
                    }

                    for (int i = 0; i < 4; ++i) {
                        int ny = y + dirs[i][0];
                        int nx = x + dirs[i][1];

                        if (ny >= 0 && ny < h && nx >= 0 && nx < w &&
                            grid.getCell(nx,ny).getCellType() == empty && !visited[ny][nx]) {
                            visited[ny][nx] = true;
                            q.push({ny, nx});
                        }
                    }
                }
                return false;
            }

            virtual common::const_map_ptr generateImpl() = 0;

            void callGameRunnerAsync(common::const_map_ptr mp, callback_game_result cb) {
                struct Task {
                    common::const_map_ptr mp;
                    callback_game_result cb;
                    void operator()() {
                        auto result = game_runner::runGame(*mp);
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


    //x-- x-- x--
    //--- #-- ---
    //--- --- #--
    class CombinationsGenerator : public MapGenerator {
        public:
            CombinationsGenerator(int h, int w) : MapGenerator(w, h) {};

            virtual common::const_map_ptr generateImpl() {
                while(++cur_comb != max_comb) {

                    auto mp = std::make_unique<common::GenericMap>(h_, w_);
                    std::bitset<common::MAX_MAP_WIDTH * common::MAX_MAP_HEIGHT-2> bt(cur_comb<<1);
                    for(int i=0; i < bt.size(); i++) {
                        int x = i%h_;
                        int y = i/h_;
                        if(bt[i])
                            mp->cell(y,x) = common::Cell(common::CellType::Wall);
                    }

                    if(!isPathExists(*mp))
                        continue;

                    mp->cell(0,0).setCellType(common::CellType::Bug);

                    //mp->print();

                    return mp;
                }
                return nullptr;
            }
        private:
            int proc = -1;
            int pre_proc = -1;
            size_t cur_comb = 0;
            size_t max_comb = 2;//std::pow(2, h_*w_-2);
    };
}