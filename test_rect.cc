#include <iostream>
#include <array>
#include <tuple>
#include <catch2/catch.hpp>
#include "rect.h"
#include "utils.h"

namespace {
std::array intersection_pairs = {
    // Rect a, Rect b, Rect collision_box
    std::make_tuple(Rect(0, 0, 2, 2), Rect(1, 1, 2, 2), Rect(1, 1, 1, 1)),
    std::make_tuple(Rect(0, 1, 3, 1), Rect(1, 0, 1, 3), Rect(1, 1, 1, 1)),
    std::make_tuple(Rect(0, 0, 4, 4), Rect(2, 3, 3, 4), Rect(2, 3, 2, 1)),
    std::make_tuple(Rect(0, 1, 2, 2), Rect(1, 0, 2, 2), Rect(1, 1, 1, 1)),
    std::make_tuple(Rect(0, 0, 4, 4), Rect(1, 1, 2, 2), Rect(1, 1, 2, 2)),
};
}

TEST_CASE("area()", "[rect]") {
    REQUIRE(Rect(0, 0, 1, 1).area() == 1);
    REQUIRE(Rect(0, 0, 2, 2).area() == 4);
    REQUIRE(Rect(0, 0, 3, 4).area() == 12);
    REQUIRE(Rect(0, 0, 4, 3).area() == 12);
    REQUIRE(Rect(1, 1, 4, 3).area() == 12);
}

TEST_CASE("intersects()", "[rect]") {
    for (const auto &[a, b, _]: intersection_pairs) {
        REQUIRE(a.intersects(b) == true);
        REQUIRE(b.intersects(a) == true);
    }

    REQUIRE(Rect(1, 1, 1, 1).intersects(Rect(4, 4, 2, 2)) == false);
}

TEST_CASE("collision_box()", "[rect]") {
    for (const auto &[a, b, c]: intersection_pairs) {
        REQUIRE(collision_box(a, b) == c);
        REQUIRE(collision_box(b, a) == c);
    }

    Rect r(0, 0, 2, 2);
    REQUIRE(collision_box(r, r) == r);
}

TEST_CASE("subtraction", "[rect]") {
    for (const auto &[a, b, _]: intersection_pairs) {
        auto result = a - b;
        const auto b_a = b - a;
        std::copy(b_a.cbegin(), b_a.cend(), std::back_inserter(result));
        const std::vector tmp = {a, b};
        REQUIRE(bounding_box(tmp.cbegin(), tmp.cend()) == bounding_box(result.cbegin(), result.cend()));
        REQUIRE(sum_areas(result.cbegin(), result.cend()) + collision_box(a, b).area() == total_area(tmp.cbegin(), tmp.cend()));
    }

    const Rect a(0, 0, 2, 2);
    REQUIRE((a - a).size() == 0);
}
