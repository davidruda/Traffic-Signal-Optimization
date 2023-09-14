#ifndef SIMULATION_SCHEDULE_HPP
#define SIMULATION_SCHEDULE_HPP

#include <cassert>
#include <optional>
#include <unordered_map>
#include <vector>

// TODO: add comments with explanations of the datastructures (they're complex)

namespace simulation {
struct TimeInterval {
public:
    TimeInterval(size_t begin, size_t end)
        : begin_(begin), end_(end) {
        assert(begin <= end);
    }

    size_t begin() const {
        return begin_;
    }

    size_t end() const {
        return end_;
    }

    size_t duration() const {
        return end_ - begin_;
    }

    void set_interval(size_t begin, size_t end);

private:
    size_t begin_;
    size_t end_;
};

class Schedule {
public:
    Schedule() = default;

    size_t length() const {
        return green_lights_.size();
    }

    size_t duration() const {
        return total_duration_;
    }

    std::vector<std::pair<size_t, TimeInterval>> green_lights() const;
    void add_street(size_t street_id, size_t green_light_duration);
    std::optional<size_t> next_green(size_t street_id, size_t time);

    std::pair<std::vector<size_t>, std::vector<size_t>> get_schedule() const;

    void set_schedule(const std::vector<size_t> &times, const std::vector<size_t> &order);

    void reset();

private:
    size_t total_duration_{};
    // mapping from street_id to index in green_lights
    std::unordered_map<size_t, size_t> street_index_;
    std::vector<TimeInterval> green_lights_;
};
}

#endif
