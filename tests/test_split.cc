#include <array>
#include <vector>
#include <iterator>
#include <tuple>
#include <catch2/catch.hpp>
#include "utils.h"
#include "split_rect.h"

namespace {
template<typename T> auto
make_test_cases() noexcept {
    return std::array<std::vector<Rect<T>>, 5> {
        std::vector{
            Rect<T>
            {0, 0, 2, 2},
            {1, 1, 2, 2},
        },
        {
            {1, 1, 2, 2},
            {9, 1, 2, 2},
            {1, 7, 2, 2},
            {9, 7, 2, 2},
            {0, 4, 4, 2},
            {8, 4, 4, 2},
            {2, 2, 8, 6},
            {5, 0, 2, 10},
        },
        {
            {1, 1, 3, 3},
            {2, 2, 3, 3},
            {3, 0, 3, 3},
        },
        {
            {0, 0, 4, 4},
            {1, 1, 2, 2},
        },
        {
            {3, 1, 4, 3},
            {4, 5, 4, 2},
            {1, 4, 2, 4},
            {4, 5, 2, 2},
            {5, 4, 4, 4},
        },
    };
}

template<typename T> void
test_split_rectangles() {
    for (const auto &tc: make_test_cases<T>()) {
        auto tbegin = tc.cbegin();
        auto tend = tc.cend();

        std::vector<Rect<T>> result;
        split_rectangles(tbegin, tend, std::back_inserter(result));
        auto rbegin = result.cbegin();
        auto rend = result.cend();

        REQUIRE_FALSE(any_intersection(rbegin, rend));
        REQUIRE(sum_areas(rbegin, rend) == total_area(tbegin, tend));
        REQUIRE(bounding_box(rbegin, rend) == bounding_box(tbegin, tend));
    }
}

template<typename T> void
test_ordering() {
    const std::array<Event<T>, 10> events{
        Event<T>{Rect<T>
         {0, 0, 2, 1}, EventType::ENTER},
        {{0, 1, 1, 1}, EventType::ENTER},
        {{0, 1, 1, 1}, EventType::LEAVE},
        {{1, 1, 1, 1}, EventType::ENTER},
        {{1, 2, 2, 1}, EventType::ENTER},
        {{0, 0, 2, 1}, EventType::LEAVE},
        {{1, 1, 1, 1}, EventType::LEAVE},
        {{2, 1, 1, 1}, EventType::ENTER},
        {{2, 1, 1, 1}, EventType::LEAVE},
        {{1, 2, 2, 1}, EventType::LEAVE},
    };

    std::priority_queue<Event<T>> prio;
    for (const auto &e: events)
        prio.push(e);

    for (const auto &e: events) {
        REQUIRE(e == prio.top());
        prio.pop();
    }
}

template<typename T> void
test_no_intersection() {
    std::vector recs{
        Rect<T>
        {0, 0, 1, 1},
        {2, 2, 1, 1},
        {3, 3, 1, 1},
    };

    std::sort(recs.begin(), recs.end());
    std::vector<Rect<T>> result;
    split_rectangles(recs.cbegin(), recs.cend(), std::back_inserter(result));
    std::sort(result.begin(), result.end());
    REQUIRE(result == recs);
}

template<typename T> void
test_adjacent() {
    std::vector recs{
        Rect<T>
        {0, 0, 1, 1},
        {0, 1, 1, 1},
        {1, 0, 1, 1},
    };

    std::sort(recs.begin(), recs.end());
    std::vector<Rect<T>> result;
    split_rectangles(recs.cbegin(), recs.cend(), std::back_inserter(result));
    std::sort(result.begin(), result.end());
    REQUIRE(result == recs);
}
}

TEST_CASE("priority queue", "[split]") {
    test_ordering<int>();
    test_ordering<unsigned int>();
}

TEST_CASE("split_rectangles", "[split]") {
    test_split_rectangles<int>();
    test_split_rectangles<unsigned int>();
}

TEST_CASE("no intersection", "[split]") {
    test_no_intersection<int>();
    test_no_intersection<unsigned int>();
}

TEST_CASE("adjacent rectangles", "[split]") {
    test_adjacent<int>();
    test_adjacent<unsigned int>();
}
