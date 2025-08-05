#pragma once

namespace lines {

struct MoveOnly {
    MoveOnly() = default;

    MoveOnly(const MoveOnly& other) = delete;
    MoveOnly& operator=(const MoveOnly& other) = delete;

    MoveOnly(MoveOnly&& other) = default;
    MoveOnly& operator=(MoveOnly&& other) = default;
};

}  // namespace lines
