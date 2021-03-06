#pragma once

#include <cstdint>
#include <array>
#include <iterator>
#include <memory>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <type_traits>
#include <set>
#include <boost/icl/interval_map.hpp>
#include <boost/pool/pool_alloc.hpp>

#include "rect.h"

enum class EventType : std::uint8_t {
    ENTER,
    LEAVE,
};

template<typename T>
struct Event {
    // The the x value used by operator< to avoid recomputing
    // it in every comparison.
    T xcmp;
    EventType type;
    std::uint64_t id;
    Rect<T> rect;

    constexpr Event(const Rect<T> &r, EventType type, std::uint64_t id = 0) noexcept
        : xcmp(type == EventType::ENTER ? r.x : r.x2), type(type), id(id), rect(r)
    {}

    constexpr Event() noexcept
        : type(EventType::ENTER), id(0)
    {}
};

template<typename Os> Os&
operator<<(Os &os, EventType evt) {
    os << "EventType::" << (evt == EventType::ENTER ? "ENTER" : "LEAVE");
    return os;
}

template<typename Os, typename T> Os&
operator<<(Os &os, const Event<T> &evt) {
    os << "Event(" << evt.rect << ", " << evt.type << ")";
    return os;
}

// the logic here is inverted because priority_queue is a max heap
template<typename T> constexpr bool
operator<(const Event<T> &lhs, const Event<T> &rhs) noexcept {
    const auto lhs_x = lhs.xcmp;
    const auto rhs_x = rhs.xcmp;
    if (lhs_x == rhs_x) {
        if (lhs.type == rhs.type)
            return lhs.rect.y < rhs.rect.y;
        return lhs.type == EventType::LEAVE;
    }

    return lhs_x < rhs_x;
}

// interval_map requires this
template<typename T> constexpr bool
operator==(const Event<T> &lhs, const Event<T> &rhs) noexcept {
    return lhs.type == rhs.type && lhs.id == rhs.id;
}

template<typename T> constexpr bool
operator!=(const Event<T> &lhs, const Event<T> &rhs) noexcept {
    return !(lhs == rhs);
}

template<typename T>
using interval_allocator = boost::fast_pool_allocator<T>;

template<
    typename Iterator,
    typename OutIterator
>
OutIterator split_rectangles(Iterator begin, Iterator end, OutIterator result) {
    using rect_t = std::remove_cv_t<std::remove_reference_t<decltype(*begin)>>;
    using size_type = decltype(begin->x);
    using event_t = Event<size_type>;

    // queue of events
    std::multiset<
        event_t,
        std::less<event_t>,
        boost::fast_pool_allocator<event_t>
    > events;

    // control the events of rects that are no longer valid
    // this is necessary because a rect becomes invalid (because
    // we splitted it) in an enter event, we should mark the leave
    // event of this specific rectangle as invalid as well
    std::unordered_set<std::uint64_t> removed;

    std::uint64_t cur_id = 0;
    // create an enter and leave events for the rectangle
    auto add_event = [&] (const rect_t &r) {
        const auto id = ++cur_id;
        events.emplace(r, EventType::ENTER, id);
        events.emplace(r, EventType::LEAVE, id);
    };

    auto new_interval = [](const rect_t &r) {
        return boost::icl::interval<size_type>::right_open(r.y, r.y2);
    };

    std::for_each(begin, end, [&] (const auto &r) {
        add_event(r);
    });

    boost::icl::interval_map<
        size_type,
        event_t,
        boost::icl::partial_absorber,
        ICL_COMPARE_INSTANCE(ICL_COMPARE_DEFAULT, size_type),
        ICL_COMBINE_INSTANCE(boost::icl::inplace_plus, size_type),
        ICL_SECTION_INSTANCE(boost::icl::inter_section, size_type),
        ICL_INTERVAL_INSTANCE(ICL_INTERVAL_DEFAULT, size_type,
            ICL_COMPARE_INSTANCE(ICL_COMPARE_DEFAULT, size_type)),
        interval_allocator
    > intervals;

    // the rects difference operations produces no more than 8 new rectangles
    std::array<rect_t, 8> diffs;

    while (!events.empty()) {
        auto it = events.begin();
        for (; it != events.end(); ++it) {
            const event_t ev(*it);

            // skip if the rectangle was previously splitted
            auto it2 = removed.find(ev.id);
            if (it2 != removed.end())
                continue;

            const auto interval(new_interval(ev.rect));

            if (ev.type == EventType::ENTER) {
                auto intersection(intervals.find(interval));

                if (intersection != intervals.end()) {
                    const auto &rintersect = intersection->second;

                    // Add the collision_box and all the split rectangles to
                    // the list of rectangles to process
                    auto col_box(collision_box(ev.rect, rintersect.rect));

                    auto dend = difference(ev.rect, rintersect.rect, diffs.begin());
                    dend = difference(rintersect.rect, ev.rect, dend);

                    events.erase(events.begin(), ++it);

                    add_event(col_box);
                    std::for_each(diffs.begin(), dend, [&] (const auto &r) {
                        // rectangles that lie behind the collision box can't
                        // intersect with any other
                        if (r.x2 > col_box.x)
                            add_event(r);
                        else
                            *result++ = r;
                    });

                    // the rectangles associated with these events no longer exist,
                    // so mark them as removed
                    removed.insert(ev.id);
                    removed.insert(rintersect.id);

                    // this interval no longer exists
                    intervals.erase(intersection->first);

                    break;
                } else {
                    intervals.insert(std::make_pair(interval, ev));
                }
            } else {
                *result++ = ev.rect;
                intervals.erase(interval);
            }
        }

        if (it == events.end())
            break;
    }

    return result;
}

