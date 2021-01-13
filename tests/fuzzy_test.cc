#include <random>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <iostream>
#include <assert.h>

#include "utils.h"
#include "split_rect.h"

int main() {
    using dist_t = std::uniform_int_distribution<int>;
    using rect_t = Rect<typename dist_t::result_type>;

    std::default_random_engine g;
    dist_t d(1, 100);

    for (;;) {
        const auto n = d(g);
        std::cout << "N=" << n << '\n';

        std::vector<rect_t> recs;
        std::generate_n(std::back_inserter(recs), n, [&] {
            return rect_t(d(g), d(g), d(g), d(g));
        });

        auto tb = recs.cbegin(); // test begin
        auto te = recs.cend(); // test end

        std::vector<rect_t> result;
        split_rectangles(tb, te, std::back_inserter(result));

        auto rb = result.cbegin(); // result begin
        auto re = result.cend(); // result end

        assert(!any_intersection(rb, re));
        assert(bounding_box(rb, re) == bounding_box(tb, te));
        assert(total_area(tb, te) == std::accumulate(rb, re, 0, [](auto acc, const auto &r) {
            return acc + r.area();
        }));
    }

    return 0;
}
