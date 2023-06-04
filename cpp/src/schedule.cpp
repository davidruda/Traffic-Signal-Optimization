
#include "schedule.hpp"

Schedule::Schedule(int intersection_id)
    : intersection_id_(intersection_id), duration_(0), last_used_time_(-1) {}

int Schedule::duration() const {
    return duration_;
}

void Schedule::add_street(int street_id, int green_light_duration) {
    green_light_ranges_.emplace(street_id, std::views::iota(duration_, duration_ + green_light_duration));
    duration_ += green_light_duration;
}

std::optional<int> Schedule::next_green(int time, const Street::Instance &street) {
    if (green_light_ranges_.contains(street.id())) {
        auto &&green_light_range = green_light_ranges_.at(street.id());
        //TODO: finish this
        if (time == street.last_used_time()) {
            ++time;
        }
        int time_normalized = time % duration_;
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