#include <catch2/catch_all.hpp>

#include <lines/fibers/api.hpp>
#include <lines/fibers/handle.hpp>
#include <lines/std/atomic.hpp>
#include <lines/time/api.hpp>
#include <lines/util/clock.hpp>

#include <vector>

#include "tp.hpp"

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
