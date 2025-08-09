#include <catch2/catch_all.hpp>

#include <exception>

#include <lines/fibers/api.hpp>
#include <lines/time/api.hpp>
#include <lines/util/defer.hpp>
#include <lines/util/logger.hpp>

#include "future.hpp"
#include "unit.hpp"

TEST_CASE("Lifetimes I") {
    lines::SchedulerRun([]() {
        auto [sync_f, sync_p] = future::GetTied<future::Unit>();
        auto [f, p] = future::GetTied<std::vector<int>>();

        auto handle = lines::Spawn([sync_p = std::move(sync_p), p = std::move(p)]() mutable {
            std::move(p).SetValue({1, 2, 3, 4, 5});

            std::move(sync_p).SetValue(future::Unit{});
        });
        lines::Defer defer([&handle]() {
            handle.join();
        });

        std::move(sync_f).Get();

        auto numbers = std::move(f).Get();
        for (size_t i = 0; i < numbers.size(); ++i) {
            REQUIRE(numbers[i] == i + 1);
        }
    });
}

TEST_CASE("Lifetimes II") {
    lines::SchedulerRun([]() {
        auto [sync_f, sync_p] = future::GetTied<future::Unit>();
        bool set_with_dead_future = false;

        std::optional<lines::Handle> handle;
        {
            auto [f, p] = future::GetTied<std::vector<int>>();

            handle = lines::Spawn(
                [sync_f = std::move(sync_f), p = std::move(p), &set_with_dead_future]() mutable {
                    std::move(sync_f).Get();

                    std::move(p).SetValue({1, 2, 3, 4, 5});
                    set_with_dead_future = true;
                });
        }

        std::move(sync_p).SetValue(future::Unit{});

        handle->join();
        REQUIRE(set_with_dead_future);
    });
}
