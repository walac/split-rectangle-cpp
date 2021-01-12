#include <iostream>
#include <array>
#include <tuple>
#include <catch2/catch.hpp>
#include "utils.h"
#include "rect.h"

namespace {
template<typename T> auto
make_test_cases() noexcept {
    return std::array {
        // Rect a, Rect b, Rect collision_box
        std::make_tuple(Rect(0, 0, 2, 2), Rect(1, 1, 2, 2), Rect(1, 1, 1, 1)),
        std::make_tuple(Rect(0, 1, 3, 1), Rect(1, 0, 1, 3), Rect(1, 1, 1, 1)),
        std::make_tuple(Rect(0, 0, 4, 4), Rect(2, 3, 3, 4), Rect(2, 3, 2, 1)),
        std::make_tuple(Rect(0, 1, 2, 2), Rect(1, 0, 2, 2), Rect(1, 1, 1, 1)),
        std::make_tuple(Rect(0, 0, 4, 4), Rect(1, 1, 2, 2), Rect(1, 1, 2, 2)),
    };
}

template<typename T> void
test_area() noexcept {
    REQUIRE(Rect<T>(0, 0, 1, 1).area() == 1);
    REQUIRE(Rect<T>(0, 0, 2, 2).area() == 4);
    REQUIRE(Rect<T>(0, 0, 3, 4).area() == 12);
    REQUIRE(Rect<T>(0, 0, 4, 3).area() == 12);
    REQUIRE(Rect<T>(1, 1, 4, 3).area() == 12);
}

template<typename T> void
test_intersects() noexcept {
    for (const auto &[a, b, _]: make_test_cases<T>()) {
        REQUIRE(a.intersects(b) == true);
        REQUIRE(b.intersects(a) == true);
    }

    REQUIRE(Rect<T>(1, 1, 1, 1).intersects(Rect<T>(4, 4, 2, 2)) == false);
}

template<typename T> void
test_collision_box() noexcept {
    for (const auto &[a, b, c]: make_test_cases<T>()) {
        REQUIRE(collision_box(a, b) == c);
        REQUIRE(collision_box(b, a) == c);
    }

    Rect<T> r(0, 0, 2, 2);
    REQUIRE(collision_box(r, r) == r);
}

template<typename T> void
test_subtraction() {
    for (const auto &[a, b, _]: make_test_cases<T>()) {
        auto result = a - b;
        const auto b_a = b - a;
        std::copy(b_a.cbegin(), b_a.cend(), std::back_inserter(result));
        const std::vector tmp = {a, b};
        REQUIRE(bounding_box(tmp.cbegin(), tmp.cend()) == bounding_box(result.cbegin(), result.cend()));
        REQUIRE(sum_areas(result.cbegin(), result.cend()) + collision_box(a, b).area() == total_area(tmp.cbegin(), tmp.cend()));
    }

    const Rect<T> a(0, 0, 2, 2);
    REQUIRE((a - a).size() == 0);
}
}

TEST_CASE("area()", "[rect]") {
    test_area<int>();
    test_area<unsigned int>();
}

TEST_CASE("intersects()", "[rect]") {
    test_intersects<int>();
    test_intersects<unsigned int>();
}

TEST_CASE("collision_box()", "[rect]") {
    test_collision_box<int>();
    test_collision_box<unsigned int>();
}

TEST_CASE("subtraction", "[rect]") {
    test_subtraction<int>();
    test_subtraction<unsigned int>();
}
