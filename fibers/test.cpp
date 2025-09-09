#include <catch2/catch_all.hpp>

#include <cstddef>

#include <lines/fibers/api.hpp>
#include <lines/std/atomic.hpp>

#include "fiber.hpp"


TEST_CASE("YieldUnit") {
    lines::SchedulerRun([]() {
        ThreadPool tp(1);

        WaitGroup wg;

        lines::Atomic<size_t> countdown = 2;
        std::vector<size_t> steps;

        wg.Add(1);
        api::Spawn(
            [&countdown, &steps]() {
                countdown.fetch_sub(1);
                while (countdown.load() != 0) {
                    api::Yield();
                }

                steps.push_back(0);

                api::Yield();

                steps.push_back(0);

                api::Yield();

                steps.push_back(0);
            },
            &tp);

        api::Spawn(
            [&countdown, &steps]() {
                countdown.fetch_sub(1);
                while (countdown.load() != 0) {
                    api::Yield();
                }

                steps.push_back(1);

                api::Yield();

                steps.push_back(1);

                api::Yield();

                steps.push_back(1);
            },
            &tp);

        tp.Wait();

        REQUIRE(steps.size() == 6);

        auto it = steps.begin();
        size_t step;
        do {
            step = *it;
            ++it;
            REQUIRE((it == steps.end() || step != *it));
        } while (it != steps.end());
    });
}

TEST_CASE("FiberParallel") {
    lines::SchedulerRun([]() {
        const size_t num_threads = 30;

        ThreadPool tp(num_threads);

        lines::Atomic<size_t> countdown = num_threads;
        for (size_t i = 0; i < num_threads; ++i) {
            api::Spawn(
                [&countdown]() {
                    countdown.fetch_sub(1);
                    while (countdown.load() != 0) {
                        api::Yield();
                    }
                },
                &tp);
        }

        tp.Wait();
        REQUIRE(countdown == 0);
    });
}

