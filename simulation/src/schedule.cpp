
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

std::optional<size_t> Schedule::next_green(size_t time, const Street::Instance &street) {
    if (green_light_ranges_.contains(street.id())) {
        auto &&green_light_range = green_light_ranges_.at(street.id());
        //TODO: finish this and FIX THIS
        if (time == street.last_used_time()) {
            ++time;
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
    return std::nullopt;
}

Schedule::iterator Schedule::begin() {
    return green_light_ranges_.begin();
}

Schedule::iterator Schedule::end() {
    return green_light_ranges_.end();
}

Schedule::const_iterator Schedule::begin() const {
    return green_light_ranges_.begin();
}

Schedule::const_iterator Schedule::end() const {
    return green_light_ranges_.end();
}

Schedule::const_iterator Schedule::cbegin() const {
    return green_light_ranges_.cbegin();
}

Schedule::const_iterator Schedule::cend() const {
    return green_light_ranges_.cend();
}
