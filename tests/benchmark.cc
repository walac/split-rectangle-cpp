#include <random>
#include <vector>
#include <algorithm>

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "split_rect.h"

constexpr auto SEED = 13607;
constexpr auto N = 500;

template<typename T>
std::vector<Rect<T>> gen_data() {
    using rect_t = Rect<T>;

    std::default_random_engine g(SEED);
    std::uniform_int_distribution<T> d(1, 100);

    std::vector<rect_t> recs;
    std::generate_n(std::back_inserter(recs), N, [&] {
        return rect_t(d(g), d(g), d(g), d(g));
    });

    return recs;
}

TEST_CASE("split_rectangles") {
    BENCHMARK_ADVANCED("default")(Catch::Benchmark::Chronometer meter) {
        auto recs = gen_data<int>();
        std::vector<Rect<int>> result;
        auto inserter = std::back_inserter(recs);

        meter.measure([&] {
            return split_rectangles(recs.cbegin(), recs.cend(), inserter);
        });
    };

}
