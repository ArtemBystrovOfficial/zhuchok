#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <unordered_map>
#include <iostream>

#include "common.hpp"

namespace thread_utils {

class ThreadPool {
public:
    ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

    explicit ThreadPool(size_t thread_count)
        : stop(false) {
        for(size_t i = 0; i < thread_count; ++i) {
            workers.emplace_back([this] {
                int tid = std::hash<std::thread::id>{}(std::this_thread::get_id());

                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(mutex);
                        cv.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });

                        if(stop && tasks.empty()) return;

                        task = std::move(tasks.front());
                        tasks.pop();
                        cv_task_taken.notify_one();
                        thread_statistics[tid]++;
                    }
                    task();
                }
            });
        }
    }

    void enqueue(std::function<void()> task, size_t max_queue = 1000) {
        {
            std::unique_lock lock(mutex);
            cv_full.wait(lock, [this, max_queue] {
                return tasks.size() < max_queue || stop;
            });
            if(stop) return;
            tasks.push(std::move(task));
        }
        cv.notify_one();
    }

    void wait_until_task_taken() {
        std::unique_lock lock(mutex);
        cv_task_taken.wait(lock, [this] {
            return tasks.empty();
        });
    }

    void printStatistics() {
        std::cout << "THREAD STATISTICS:\n";
        for(auto & [_, val] : thread_statistics) {
            std::cout  << val << "\n";
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard lock(mutex);
            stop = true;
        }
        cv.notify_all();
        cv_full.notify_all();
        for(auto& worker : workers) {
            if(worker.joinable()) worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable cv;
    std::condition_variable cv_full;
    std::condition_variable cv_task_taken;

    std::atomic<bool> stop;
    std::unordered_map<int, size_t> thread_statistics;
};

class MaxTracker {
public:
    void update_if_better(int new_value, std::unique_ptr<common::BaseMap> new_map) {

        int current_max = max_value.load(std::memory_order_acquire);
        if (new_value <= current_max) {
            return;
        }

        std::lock_guard lock(map_mutex);

        current_max = max_value.load(std::memory_order_relaxed);
        if (new_value > current_max) {

            max_value.store(new_value, std::memory_order_release);
            best_map = std::move(new_map);

        }
    }

    std::pair<int, const common::BaseMap *> get_best() {
        std::lock_guard lock(map_mutex);

        int value = max_value.load(std::memory_order_relaxed);
        if (value == 0 || !best_map) {
            return {0, nullptr};
        }

        return {value, best_map.get()};
    }

private:
    std::atomic_int max_value{0};

    std::mutex map_mutex;
    std::unique_ptr<common::BaseMap> best_map;
};

}