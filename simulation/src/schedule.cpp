
#include "schedule.hpp"

Schedule::Schedule(size_t intersection_id)
    : intersection_id_(intersection_id), duration_(0) {}

size_t Schedule::length() const {
    return green_light_ranges_.size();
}

size_t Schedule::duration() const {
    return duration_;
}

void Schedule::add_street(size_t street_id, size_t green_light_duration) {
    green_light_ranges_.emplace(street_id, std::views::iota(duration_, duration_ + green_light_duration));
    duration_ += green_light_duration;
}

std::optional<size_t> Schedule::next_green(const Street::Instance &street, size_t time) {
    if (green_light_ranges_.contains(street.id())) {
        auto &&green_light_range = green_light_ranges_[street.id()];
        //TODO: finish this and FIX THIS
        if (street.last_used_time().has_value()) {
            if (time == street.last_used_time().value()) {
                ++time;
            }
        }
        auto time_normalized = time % duration_;
        auto start = *green_light_range.begin();
        if (time_normalized < start) {
            return (time - time_normalized) + start;
        }
        auto end = *green_light_range.end();
        if (time_normalized >= end) {
            return (time - time_normalized) + duration_ + start;
        }
        return time;
    }
    return {};
}
