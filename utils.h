#pragma once
#include <assert.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <iostream>
#include "rect.h"

template<typename ForwardIterator> auto
bounding_box(ForwardIterator begin, ForwardIterator end) noexcept {
    assert(begin != end);

    auto min_x = begin->x;
    auto min_y = begin->y;
    auto max_x2 = begin->x2();
    auto max_y2 = begin->y2();

    ++begin;

    std::for_each(begin, end, [&] (const auto &r) {
        if (r.x < min_x)
            min_x = r.x;

        if (r.y < min_y)
            min_y = r.y;

        if (r.x2() > max_x2)
            max_x2 = r.x2();

        if (r.y2() > max_y2)
            max_y2 = r.y2();
    });

    return Rect(min_x, min_y, max_x2 - min_x, max_y2 - min_y);
}

template<typename ForwardIterator> auto
total_area(ForwardIterator begin, ForwardIterator end) {
    const auto box = bounding_box(begin, end);

    using size_type = decltype(box.x);

    std::vector m(box.height, std::vector<size_type>(box.width));
    std::for_each(begin, end, [&] (const auto &r) {
        for (size_type y{}; y < r.height; ++y)
            for (size_type x{}; x < r.width; ++x)
                m[r.y + y - box.y][r.x + x - box.x] = 1;
    });

    return std::accumulate(m.cbegin(), m.cend(), size_type{}, [] (auto acc, const auto &vec) {
        return acc + std::accumulate(vec.cbegin(), vec.cend(), size_type{});
    });
}

template<typename ForwardIterator> auto
sum_areas(ForwardIterator begin, ForwardIterator end) noexcept {
    using size_type = decltype(begin->x);
    return std::accumulate(begin, end, size_type{}, [] (auto acc, const auto &r) { return acc + r.area(); });
}

template<typename ForwardIterator> bool
any_intersection(ForwardIterator begin, ForwardIterator end) noexcept {
    for (auto first = begin; first != end; ++first) {
        auto second = first;
        for (++second; second != end; ++second)
            if (first->intersects(*second))
                return true;
    }

    return false;
}

namespace std {
template<typename Os, typename T, typename Allocator> Os&
operator<<(Os &os, const vector<T, Allocator> &vec) {
    os << "std::vector(";
    std::copy(vec.cbegin(), vec.cend(), std::ostream_iterator<T>(os, ", "));
    os << ")";
    return os;
}
}
