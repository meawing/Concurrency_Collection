#include <catch2/catch_all.hpp>

#include <lines/fibers/api.hpp>
#include <lines/fibers/handle.hpp>
#include <lines/std/atomic.hpp>
#include <lines/time/api.hpp>
#include <lines/util/clock.hpp>

#include <vector>

#include "tp.hpp"

TEST_CASE("QueueUnit") {
    lines::SchedulerRun([]() {
        MPMCBlockingUnboundedQueue<int> queue;

        const size_t num_fibers = 10;
        std::vector<lines::Handle> handles;
        lines::Atomic<size_t> counter = num_fibers;
        for (size_t i = 0; i < num_fibers; ++i) {
            handles.emplace_back(lines::Spawn([&queue, &counter, i]() {
                lines::SleepFor(200ms);

                queue.Push(i + 1);

                if (counter.fetch_sub(1) == 1) {
                    queue.Close();
                }
            }));
        }

        size_t sum = 0;
        std::optional<size_t> data;
        while ((data = queue.Pop())) {
            sum += *data;
        }

        REQUIRE(sum == num_fibers * (num_fibers + 1) / 2);

        for (auto& handle : handles) {
            handle.join();
        }
    });
}

TEST_CASE("QueueClose") {
    lines::SchedulerRun([]() {
        MPMCBlockingUnboundedQueue<int> queue;

        const size_t num_fibers = 10;
        std::vector<lines::Handle> handles;
        lines::Atomic<size_t> counter = num_fibers;
        for (size_t i = 0; i < num_fibers; ++i) {
            handles.emplace_back(lines::Spawn([&queue, &counter, i]() {
                lines::SleepFor(200ms);

                if (counter.fetch_sub(1) == num_fibers / 2) {
                    queue.Close();
                }

                queue.Push(i + 1);
            }));
        }

        size_t sum = 0;
        std::optional<size_t> data;
        while ((data = queue.Pop())) {
            sum += *data;
        }

        REQUIRE(sum < num_fibers * (num_fibers + 1) / 2);

        for (auto& handle : handles) {
            handle.join();
        }
    });
}

TEST_CASE("QueueStress") {
    lines::SchedulerRun(
        []() {
            const size_t num_threads = 8;
            const size_t stream_size = 50000;

            MPMCBlockingUnboundedQueue<size_t> stream;
            std::vector<std::vector<size_t>> answers(num_threads);

            auto source = lines::Spawn([&stream]() {
                for (size_t i = 0; i < num_threads * stream_size; ++i) {
                    stream.Push(i);
                }
            });

            std::vector<lines::Handle> consumers;
            for (size_t tid = 0; tid < num_threads; ++tid) {
                consumers.emplace_back([&ans = answers[tid], &stream]() {
                    for (size_t num = 0; num < stream_size; ++num) {
                        ans.push_back(*stream.Pop());
                    }
                });
            }

            for (auto& consumer : consumers) {
                consumer.join();
            }
            source.join();

            size_t sum = 0;
            for (const auto& ans : answers) {
                for (size_t num : ans) {
                    sum += num;
                }
            }
            REQUIRE(sum == (num_threads * stream_size) * (num_threads * stream_size - 1) / 2);
        },
        /*num_runs=*/4);
}

TEST_CASE("WaitGroup") {
    lines::SchedulerRun([]() {
        WaitGroup wg;

        const size_t num_fibers = 10;
        std::vector<lines::Handle> handles;
        lines::Atomic<size_t> sum;
        for (size_t i = 0; i < num_fibers; ++i) {
            wg.Add(1);

            handles.emplace_back(lines::Spawn([&wg, &sum, i]() {
                sum.fetch_add(i);
                wg.Done();
            }));
        }

        wg.Wait();

        REQUIRE(sum.load() == num_fibers * (num_fibers - 1) / 2);

        for (auto& handle : handles) {
            handle.join();
        }
    });
}

TEST_CASE("ThreadPoolUnit") {
    lines::SchedulerRun([]() {
        ThreadPool pool(2);

        REQUIRE(!ThreadPool::This());

        lines::Atomic<int> counter = 0;

        pool.Submit([&counter]() {
            lines::SleepFor(100ms);
            counter += 1;
        });

        pool.Submit([&counter]() {
            lines::SleepFor(200ms);

            ThreadPool::This()->Submit([&counter]() {
                counter += 1;
            });
        });

        pool.Wait();

        REQUIRE(counter == 2);
    });
}

TEST_CASE("ThreadPoolStress") {
    lines::SchedulerRun([]() {
        lines::CpuClock cpu;
        lines::WallClock wall;

        cpu.Start();
        wall.Start();

        const size_t num_threads = 8;
        ThreadPool pool(num_threads);

        const size_t num_tasks = 100;
        lines::Atomic<size_t> sum;
        for (size_t i = 0; i < num_tasks; ++i) {
            pool.Submit([&sum]() {
                if ((sum.fetch_add(1) & 1) == 0) {
                    lines::SleepFor(50ms);
                }
            });
        }

        pool.Wait();

        auto cpu_time = cpu.Finish();
        auto wall_time = wall.Finish();

        REQUIRE(lines::IsClockLess(cpu_time * num_threads, wall_time));
        REQUIRE(sum.load() == num_tasks);
    });
}
