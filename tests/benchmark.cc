#include <random>
#include <vector>
#include <algorithm>

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "split_rect.h"

constexpr auto SEED = 13607;
constexpr auto N = 100;

TEST_CASE("split") {
    BENCHMARK_ADVANCED("split_rectangles")(Catch::Benchmark::Chronometer meter) {
        using rect_t = Rect<int>;

        std::default_random_engine g(SEED);
        std::uniform_int_distribution d(1, 100);

        std::vector<rect_t> recs;
        std::generate_n(std::back_inserter(recs), N, [&] {
            return rect_t(d(g), d(g), d(g), d(g));
        });

        std::vector<rect_t> result;

        meter.measure([&] {
            split_rectangles(recs.cbegin(), recs.cend(), std::back_inserter(result));
            return result;
        });
    };
}
