#pragma once

#include "common.hpp"
#include "GameRunner.hpp"
#include "ThreadUtils.hpp"

#include <functional>
#include <iostream>
#include <bitset>
#include <bit>
#include <cmath>
#include <algorithm>
#include <random>

namespace map_generator {
    class MapGenerator {
        using callback_game_result = std::function<void(int, common::const_map_ptr)>;

        public:
            MapGenerator(int w, int h): h_(h), w_(w) {};

            common::const_map_ptr generate() {
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
                //pool_.printStatistics();
                //std::cout << "BEST MAP:\n";
                best_map->print(true);
                //std::cout << "BEST SCORE:\n";
                std::cout << best_score << std::endl;

                return best_map;
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

    #include <algorithm>

    class FixedWallsGenerator : public MapGenerator {
    public:
        FixedWallsGenerator(int h, int w, int walls_count = 7, int max_count = 0)
            : MapGenerator(w, h), walls_count_(walls_count), generated_(false), max_count_(max_count) {
            total_cells_ = h * w - 2;

            // Инициализируем маску правильно
            combination_mask_.clear();

            // Сначала все false

            for(int i = 0; i < total_cells_ - walls_count_; ++i) {
                combination_mask_.push_back(false);
            }

            // Потом все true
            for(int i = 0; i < walls_count_; ++i) {
                combination_mask_.push_back(true);
            }
        }

        virtual common::const_map_ptr generateImpl() {
            // Если уже все сгенерировали
            if(generated_) {
                return nullptr;
            }

            // Генерируем все комбинации
            do {
                auto mp = std::make_unique<common::GenericMap>(h_, w_);

                // Заполняем карту

                for(int i = 0; i < total_cells_; i++) {
                    if(combination_mask_[i]) {
                        int x = i % h_;
                        int y = i / h_;
                                mp->cell(y, x) = common::Cell(common::CellType::Wall);
                    }
                }

                if (++count_ == max_count_)
                    return nullptr;

                // Готовим следующую комбинацию
                if(!std::next_permutation(combination_mask_.begin(), combination_mask_.end())) {
                    generated_ = true;
                }

                // Проверяем путь
                if(!isPathExists(*mp)) {
                    if(generated_) break;
                    continue;
                }

                mp->cell(0, 0).setCellType(common::CellType::Bug);
                return mp;

            } while(true);

            return nullptr;
        }

    private:
        int walls_count_;
        int total_cells_;
        std::vector<bool> combination_mask_;
        bool generated_;
        int max_count_;
        int count_ = 0;
    };


    class RandomizeGenerator : public MapGenerator {
        public:
            RandomizeGenerator(int h, int w, int count, int proc) : MapGenerator(w, h) ,count_(count), proc_(proc) {
                srand(time(0));
            };

            virtual common::const_map_ptr generateImpl() {
                while(true) {

                     auto mp = std::make_unique<common::GenericMap>(h_, w_);
                    for(int y = 0; y < h_; y++) {
                        for(int x = 0; x < w_; x++) {
                            int random_value = rand() % 100;
                            if(random_value < proc_) {
                                mp->cell(x, y) = common::Cell(common::CellType::Wall);
                            }
                            else {
                                mp->cell(x, y) = common::Cell(common::CellType::Empty);
                            }
                        }
                    }
                    if(++cur > count_)
                        break;

                    if(!isPathExists(*mp))
                        continue;

                    mp->cell(0,0).setCellType(common::CellType::Bug);
                    mp->cell(w_-1,h_-1).setCellType(common::CellType::Empty);

                    return mp;
                }
                return nullptr;
            }
        private:
            int proc_ = 0;
            int count_ = -1;
            int cur = 0;
    };

    class EvoltuionGenerator : public MapGenerator {
        public:
            EvoltuionGenerator(common::const_map_ptr map, int count_era, int proc, int only_right = false, int width_layer = 1)
            : MapGenerator(map->GetWidth()+width_layer,only_right ? map->GetHeight() : map->GetHeight()+width_layer), map_(std::move(map)),
            count_era_(count_era), proc_(proc), only_right_(only_right), width_layer_(width_layer)  {
                srand(time(0));
            };

            virtual common::const_map_ptr generateImpl() {
                while(true) {

                    auto mp = std::make_unique<common::GenericMap>(h_, w_);

                    for(int y = 0; y < map_->GetHeight(); y++) {
                        for(int x = 0; x < map_->GetWidth(); x++) {
                            mp->cell(x,y).setCellType(map_->getCell(x,y).getCellType());
                        }
                    }

                    for(int x = 0; x < width_layer_;x++) {
                        for(int y = 0; y < h_ - 1; y++) {
                            int random_value = rand() % 100;
                            if(random_value < proc_) {
                                mp->cell(map_->GetWidth()+x-width_layer_+1, y) = common::Cell(common::CellType::Wall);
                            }
                            else {
                                mp->cell(map_->GetWidth()+x-width_layer_+1, y) = common::Cell(common::CellType::Empty);
                            }
                        }
                    }

                    if(!only_right_) {
                        for(int y = 0; y < width_layer_;y++) {
                            for(int x = 0; x < w_ - 1; x++) {
                                int random_value = rand() % 100;
                                if(random_value < proc_) {
                                    mp->cell(x, map_->GetHeight()+y-width_layer_+1) = common::Cell(common::CellType::Wall);
                                }
                                else {
                                    mp->cell(x, map_->GetHeight()+y-width_layer_+1) = common::Cell(common::CellType::Empty);
                                }
                            }
                        }
                    }


                    if(++cur > count_era_)
                        break;

                    if(!isPathExists(*mp))
                        continue;

                    mp->cell(0,0).setCellType(common::CellType::Bug);
                    mp->cell(w_-1,h_-1).setCellType(common::CellType::Empty);

                    return mp;
                }
                return nullptr;
            }

        private:
            common::const_map_ptr map_;
            int proc_ = 0;
            int count_era_ = -1;
            int cur = 0;
            bool only_right_;
            int width_layer_;
    };

}