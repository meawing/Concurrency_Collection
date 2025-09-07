#include <catch2/catch_all.hpp>

#include <vector>

#include "coroutine.hpp"

void CheckSteps(const std::vector<int>& expected, std::vector<int>& steps) {
    REQUIRE(steps.size() == expected.size());
    for (size_t i = 0; i < steps.size(); ++i) {
        REQUIRE(steps[i] == expected[i]);
    }
}

void H(std::vector<int>& steps) {
    steps.push_back(1);
    Coroutine::Suspend();
    steps.push_back(2);
}

void G(std::vector<int>& steps) {
    H(steps);
}

void F(std::vector<int>& steps) {
    G(steps);
}

TEST_CASE("Stackful") {
    std::vector<int> steps;
    Coroutine coro([&steps]() {
        F(steps);
    });

    steps.push_back(3);

    coro.Resume();

    steps.push_back(4);

    coro.Resume();

    steps.push_back(5);

    REQUIRE(coro.IsCompleted());

    const std::vector<int> expected = {3, 1, 4, 2, 5};
    CheckSteps(expected, steps);
}

TEST_CASE("Folded coroutine") {
    std::vector<int> steps;
    Coroutine coro([&steps]() {
        Coroutine folded_coro([&steps]() {
            steps.push_back(1);
            Coroutine::Suspend();

            steps.push_back(2);
        });

        steps.push_back(3);
        folded_coro.Resume();

        steps.push_back(4);
        Coroutine::Suspend();

        steps.push_back(5);
        folded_coro.Resume();

        REQUIRE(folded_coro.IsCompleted());
        steps.push_back(6);
    });

    steps.push_back(7);
    coro.Resume();

    steps.push_back(8);
    coro.Resume();

    REQUIRE(coro.IsCompleted());

    const std::vector<int> expected = {7, 3, 1, 4, 8, 5, 2, 6};
    CheckSteps(expected, steps);
}
