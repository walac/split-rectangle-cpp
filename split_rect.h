#pragma once

#include <cstdint>
#include <iterator>
#include <memory>
#include <unordered_set>
#include <queue>
#include <type_traits>
#include <boost/icl/interval_map.hpp>

#include "rect.h"

enum class EventType : std::uint8_t {
    ENTER,
    LEAVE,
};

template<typename T>
struct Event {
    Rect<T> rect;
    std::uint64_t id;
    EventType type;

    constexpr Event(const Rect<T> &r, EventType type, std::uint64_t id = 0) noexcept
        : rect(r), id(id), type(type)
    {}

    constexpr Event() noexcept
        : id(0), type(EventType::ENTER)
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
    const auto lhs_x = lhs.type == EventType::ENTER ? lhs.rect.x : lhs.rect.x2();
    const auto rhs_x = rhs.type == EventType::ENTER ? rhs.rect.x : rhs.rect.x2();

    if (lhs_x == rhs_x) {
        if (lhs.type == rhs.type)
            return lhs.rect.y > rhs.rect.y;
        return rhs.type == EventType::LEAVE;
    }

    return rhs_x < lhs_x;
}

// interval_map requires this
template<typename T> constexpr bool
operator==(const Event<T> &lhs, const Event<T> &rhs) noexcept {
    return lhs.type == rhs.type && lhs.rect == rhs.rect && lhs.id == rhs.id;
}

template<typename Iterator, typename OutIterator> void
split_rectangles(Iterator begin, Iterator end, OutIterator result) {
    using rect_type = std::__remove_cvref_t<decltype(*begin)>;
    using size_type = decltype(begin->x);
    using event_type = Event<size_type>;

    // queue of events
    std::priority_queue<event_type, std::vector<event_type>> events;

    // control the events of rects that are no longer valid
    // this is necessary because a rect becomes invalid (because
    // we splitted it) in an enter event, we should mark the leave
    // event of this specific rectangle as invalid as well
    std::unordered_set<std::uint64_t> removed;

    std::uint64_t cur_id = 0;
    // create an enter and leave events for the rectangle
    auto add_event = [&] (const rect_type &r) {
        const auto id = ++cur_id;
        events.emplace(r, EventType::ENTER, id);
        events.emplace(r, EventType::LEAVE, id);
    };

    auto new_interval = [](const rect_type &r) {
        return boost::icl::interval<size_type>::right_open(r.y, r.y2());
    };

    std::for_each(begin, end, [&] (const auto &r) {
        add_event(r);
    });

    boost::icl::interval_map<size_type, event_type> intervals;
    std::vector<rect_type> diffs;
    diffs.reserve(8);

    while (!events.empty()) {
        const event_type ev(events.top());
        events.pop();

        // skip if the rectangle was previously splitted
        auto it = removed.find(ev.id);
        if (it != removed.end()) {
            // if it is a leave event, that's the last time
            // we encounter this event id
            if (ev.type == EventType::LEAVE)
                removed.erase(it);

            continue;
        }

        const auto interval(new_interval(ev.rect));

        if (ev.type == EventType::ENTER) {
            auto intersection(intervals.find(interval));

            if (intersection != intervals.end()) {
                const auto &rintersect = intersection->second;

                // Add the collision_box and all the split rectangles to
                // the list of rectangles to process
                add_event(collision_box(ev.rect, rintersect.rect));

                difference(ev.rect, rintersect.rect, diffs);
                difference(rintersect.rect, ev.rect, diffs);

                for_each(diffs.cbegin(), diffs.cend(), [&] (const auto &r) {
                    add_event(r);
                });

                diffs.clear();

                // the rectangles associated with these events no longer exist,
                // so mark them as removed
                removed.insert(ev.id);
                removed.insert(rintersect.id);

                // this interval no longer exists
                intervals.erase(intersection->first);
            } else {
                intervals.insert(std::make_pair(interval, ev));
            }
        } else {
            *result++ = ev.rect;
            intervals.erase(interval);
        }
    }
}

