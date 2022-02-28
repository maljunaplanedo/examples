#include <stdio.h>
#include <inttypes.h>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <set>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/assoc_container.hpp>

template<typename T>
using ordered_set =
__gnu_pbds::tree<
        T,
        __gnu_pbds::null_type,
        std::less<T>,
        __gnu_pbds::rb_tree_tag,
        __gnu_pbds::tree_order_statistics_node_update>;

using coordinate_t = int64_t;

struct Vector2d {
    coordinate_t x;
    coordinate_t y;

    Vector2d(coordinate_t x, coordinate_t y):
        x(x), y(y)
    {   }

    Vector2d():
        Vector2d(0, 0)
    {   }

    bool operator==(const Vector2d& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2d& other) const {
        return !(*this == other);
    }

    bool operator<(const Vector2d& other) const {
        if (x == other.x)
            return y < other.y;
        return x < other.x;
    }
};

enum class SegmentType {
    normal, vertical, point
};

struct Segment {
    coordinate_t from;
    coordinate_t to;
    coordinate_t k;
    coordinate_t b;
    SegmentType type;

    coordinate_t alpha() const {
        return to - from;
    }

    Segment(Vector2d first, Vector2d second) {
        if (first == second) {
            type = SegmentType::point;
            from = to = first.x;
            k = 0;
            b = first.y;
        } else if (first.x == second.x) {
            type = SegmentType::vertical;
            from = first.y;
            to = second.y;
            k = first.x;
            b = 0;

            if (from > to)
                std::swap(from, to);
        } else {
            type = SegmentType::normal;
            from = first.x;
            to = second.x;

            if (from > to) {
                std::swap(from, to);
                std::swap(first, second);
            }

            k = second.y - first.y;
            b = alpha() * first.y - k * from;
        }
    }

    coordinate_t put(coordinate_t x) const {
        return k * x + b;
    }

    coordinate_t y_from() const {
        if (type == SegmentType::point)
            return b;
        return put(from) / alpha();
    }

    coordinate_t y_to() const {
        if (type == SegmentType::point)
            return b;
        return put(to) / alpha();
    }

    Vector2d first() const {
        return {from, y_from()};
    }

    Vector2d second() const {
        return {to, y_to()};
    }

    bool operator==(const Segment& other) const {
        return first() == other.first() && second() == other.second();
    }

    bool operator!=(const Segment& other) const {
        return !(*this == other);
    }

};

coordinate_t point_to_segment(Vector2d point, const Segment& segment) {
    assert(segment.type == SegmentType::normal);
    return segment.alpha() * point.y - segment.put(point.x);
}

bool operator<(const Segment& first, const Segment& second) {
    if (first == second)
        return false;

    if (first.type == SegmentType::normal && second.type == SegmentType::normal) {
        if (first.first() == second.first())
            return point_to_segment(first.second(), second) < 0;
        if (first.second() == second.second())
            return point_to_segment(first.first(), second) < 0;
        if (first.second() == second.first())
            return true;
        if (second.second() == first.first())
            return false;

        if (point_to_segment(second.first(), first) < 0 &&
                point_to_segment(second.second(), first) < 0)
            return false;
        if (point_to_segment(second.first(), first) > 0 &&
                point_to_segment(second.second(), first) > 0)
            return true;
        if (point_to_segment(first.first(), second) < 0 &&
                point_to_segment(first.second(), second) < 0)
            return true;
        if (point_to_segment(first.first(), second) > 0 &&
                point_to_segment(first.second(), second) > 0)
            return false;
    } else if (first.type == SegmentType::point) {
        return point_to_segment(first.first(), second) <= 0;
    } else if (second.type == SegmentType::point) {
        return point_to_segment(second.first(), first) > 0;
    }
    assert(false);
}

enum class EventType {
    segment_end, segment_begin, point
};

enum class PointType {
    inside, border, outside, unknown
};

struct Event {
    EventType type;
    int number;
    coordinate_t place;

    Event(EventType type, int number, coordinate_t place):
        type(type), number(number), place(place)
    {   }
};

bool operator<(const Event& left, const Event& right) {
    if (left.place == right.place)
        return left.type < right.type;
    return left.place < right.place;
}

void solve() {
    int n, k;
    std::vector<Segment> segments;
    std::vector<Vector2d> points;
    std::vector<Event> events;
    std::vector<PointType> answer;

    scanf("%d", &n);
    Vector2d previous;
    Vector2d first;
    std::set<Vector2d> vertices;

    for (int i = 0; i < n; ++i) {
        Vector2d p;
        scanf("%ld%ld", &p.x, &p.y);
        vertices.insert(p);

        if (i != 0 && p == previous)
            continue;
        if (i == 0)
            first = p;
        else
            segments.emplace_back(previous, p);
        previous = p;
    }
    if (previous != first)
        segments.emplace_back(previous, first);

    scanf("%d", &k);
    answer.assign(k, PointType::unknown);

    for (int i = 0; i < k; ++i) {
        Vector2d p;
        scanf("%ld%ld", &p.x, &p.y);
        points.push_back(p);

        if (vertices.find(p) != vertices.end())
            answer[i] = PointType::border;
    }

    if (segments.size() < 3) {
        while (true);
        for (const auto& p: points) {
            if (p == first)
                puts("BORDER");
            else
                puts("OUTSIDE");
        }
        return;
    }

    n = static_cast<int>(segments.size());

    for (int i = 0; i < n; ++i) {
        if (segments[i].type == SegmentType::normal)
            continue;
        assert(segments[i].type == SegmentType::vertical);
        events.emplace_back(EventType::segment_begin, i, segments[i].from);
        events.emplace_back(EventType::segment_end, i, segments[i].to);
    }

    for (int i = 0; i < k; ++i) {
        if (answer[i] == PointType::unknown)
            events.emplace_back(EventType::point, i, points[i].y);
    }

    std::sort(events.begin(), events.end());
    ordered_set<coordinate_t> current_vertical;

    for (const auto& event: events) {
        if (event.type == EventType::segment_begin) {
            current_vertical.insert(segments[event.number].k);
        } else if (event.type == EventType::point) {
            int pos = static_cast<int>(current_vertical.order_of_key(points[event.number].x));

            if (pos == static_cast<int>(current_vertical.size()))
                continue;

            auto it = current_vertical.find_by_order(pos);
            if (*it == points[event.number].x)
                answer[event.number] = PointType::border;

        } else if (event.type == EventType::segment_end) {
            current_vertical.erase(segments[event.number].k);
        }
    }

    events.clear();
    for (int i = 0; i < n; ++i) {
        if (segments[i].type == SegmentType::vertical)
            continue;
        assert(segments[i].type == SegmentType::normal);
        events.emplace_back(EventType::segment_begin, i, segments[i].from);
        events.emplace_back(EventType::segment_end, i, segments[i].to);
    }

    for (int i = 0; i < k; ++i) {
        if (answer[i] == PointType::unknown)
            events.emplace_back(EventType::point, i, points[i].x);
    }

    std::sort(events.begin(), events.end());
    ordered_set<Segment> current;

    for (const auto& event: events) {
        if (event.type == EventType::segment_begin) {
            current.insert(segments[event.number]);
        } else if (event.type == EventType::point) {
            Vector2d point = points[event.number];
            int pos = static_cast<int>(current.order_of_key(Segment(point, point)));

            if (pos == static_cast<int>(current.size())) {
                answer[event.number] = PointType::outside;
                continue;
            }

            auto it = current.find_by_order(pos);
            if (point_to_segment(point, *it) == 0)
                answer[event.number] = PointType::border;
            else if (pos % 2)
                answer[event.number] = PointType::inside;
            else
                answer[event.number] = PointType::outside;
        } else if (event.type == EventType::segment_end) {
            current.erase(segments[event.number]);
        }
    }

    for (int i = 0; i < k; ++i) {
        switch (answer[i]) {
            case PointType::inside:
                puts("INSIDE");
                break;
            case PointType::outside:
                puts("OUTSIDE");
                break;
            case PointType::border:
                puts("BORDER");
                break;
            default:
                assert(false);
        }
    }
}

int main() {
    int t;
    scanf("%d", &t);
    for (int i = 0; i < t; ++i) {
        solve();
        puts("");
    }
    return 0;
}
