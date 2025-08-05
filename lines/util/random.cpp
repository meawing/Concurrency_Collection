#include <lines/util/random.hpp>
#include <thread>

#include <random>

namespace lines {

class RandomSource {
public:
    RandomSource(int seed) : generator_(seed) {
    }

    int Random(int lower, int upper) {
        dist_.param(std::uniform_int_distribution<int>::param_type(lower, upper));
        return dist_(generator_);
    }

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<int> dist_;
};

static std::atomic<size_t> id;
static thread_local RandomSource random(++id);

int Random(int upper) {
    return Random(0, upper);
}

int Random(int lower, int upper) {
    return random.Random(lower, upper);
}

bool FlipCoin() {
    return random.Random(0, 1) == 0;
}

}  // namespace lines
