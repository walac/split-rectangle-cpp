#include <random>
#include <vector>
#include <algorithm>
#include <boost/pool/pool_alloc.hpp>

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "split_rect.h"

constexpr auto SEED = 13607;
constexpr auto N = 100;

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

template<typename T>
using pool_allocator = boost::fast_pool_allocator<T>;

TEST_CASE("split_rectangles") {
    BENCHMARK_ADVANCED("default allocator")(Catch::Benchmark::Chronometer meter) {
        auto recs = gen_data<int>();
        std::vector<Rect<int>> result;
        auto inserter = std::back_inserter(recs);

        meter.measure([&] {
            return split_rectangles(recs.cbegin(), recs.cend(), inserter);
        });
    };

#if 0
    BENCHMARK_ADVANCED("boost pool allocator")(Catch::Benchmark::Chronometer meter) {
        auto recs = gen_data<int>();
        std::vector<Rect<int>> result;
        auto inserter = std::back_inserter(recs);

        meter.measure([&] {
            return split_rectangles<
                decltype(recs.cbegin()),
                decltype(inserter),
                pool_allocator
            >(recs.cbegin(), recs.cend(), std::back_inserter(result));
        });
    };
#endif
}
