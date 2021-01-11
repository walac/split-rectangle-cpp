#pragma once

#include <algorithm>
#include <vector>

template<typename T>
struct Rect {
    T x, y, width, height;

    Rect(T x, T y, T width, T height)
        : x(x), y(y), width(width), height(height)
    {}

    T x2() const noexcept {
        return x + width;
    }

    T y2() const noexcept {
        return y + height;
    }

    T area() const noexcept {
        return width * height;
    }

    bool contains(const Rect &r) const noexcept {
        return r.x >= x
            && r.y >= y
            && r.x2() <= x2()
            && r.y2() <= y2();
    }

    bool intersects(const Rect &rhs) const noexcept {
        return !(rhs.x2() <= x
            || rhs.y2() <= y
            || rhs.x >= x2()
            || rhs.y >= y2());
    }
};

template<typename T> Rect<T>
collision_box(const Rect<T> &a, const Rect<T> &b) noexcept {
    const auto x = std::max(a.x, b.x);
    const auto y = std::max(a.y, b.y);
    return Rect(
        x,
        y,
        std::min(a.x2(), b.x2()) - x,
        std::min(a.y2(), b.y2()) - y
    );
}

template<typename T> bool
intersects(const Rect<T> &a, const Rect<T> &b) noexcept {
    return !(b.x2() <= a.x
        || b.y2() <= a.y
        || b.x >= a.x2()
        || b.y >= a.y2());
}

template<typename T, typename Container> void
difference(const Rect<T> &lhs, const Rect<T> &rhs, Container &result)
    noexcept(noexcept(result.emplace_back(Rect{0, 0, 0, 0})))
{
    if (rhs.contains(lhs))
        return;

    // compute the top rectangle
    if (rhs.y > lhs.y)
        result.emplace_back(lhs.x, lhs.y, lhs.width, rhs.y - lhs.y);

    // compute the bottom rectangle
    if (lhs.y2() > rhs.y2())
        result.emplace_back(lhs.x, rhs.y2(), lhs.width, lhs.y2() - rhs.y2());

    const auto y1 = rhs.y > lhs.y ? rhs.y : lhs.y;
    const auto y2 = rhs.y2() < lhs.y2() ? rhs.y2() : lhs.y2();

    if (y2 > y1) {
        const auto rc_height  = y2 - y1;

        // compute the left rectangle
        if (rhs.x > lhs.x)
            result.emplace_back(lhs.x, y1, rhs.x - lhs.x, rc_height);

        // compute the right rectangle
        if (lhs.x2() > rhs.x2())
            result.emplace_back(rhs.x2(), y1, lhs.x2() - rhs.x2(), rc_height);
    }
}

template<typename T> bool
operator==(const Rect<T> &lhs, const Rect<T> &rhs) {
    return lhs.x == rhs.x
        && lhs.y == rhs.y
        && lhs.width == rhs.width
        && lhs.height == rhs.height;
}

template<typename T> bool
operator!=(const Rect<T> &lhs, const Rect<T> &rhs) {
    return !(lhs == rhs);
}

template<typename T> bool
operator<(const Rect<T> &lhs, const Rect<T> &rhs) noexcept {
    if (lhs.x < rhs.x)
        return true;

    if (lhs.x == rhs.x && lhs.y < rhs.y)
        return true;

    return false;
}

template<typename T> std::vector<Rect<T>>
operator-(const Rect<T> &lhs, const Rect<T> &rhs) {
    std::vector<Rect<T>> vec;
    vec.reserve(4);
    difference(lhs, rhs, vec);
    return vec;
}

template<typename Os, typename T> Os&
operator<<(Os &os, const Rect<T> &r) {
    static constexpr auto *c = ", ";
    os << "Rect(" << r.x << c << r.y << c << r.width << c << r.height << ")";
    return os;
}
