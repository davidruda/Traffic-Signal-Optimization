#include "schedule.hpp"

namespace simulation {

    void Schedule::add_street(size_t street_id, size_t green_light_duration) {
        green_lights_.emplace(street_id, TimeInterval(duration_, duration_ + green_light_duration));
        duration_ += green_light_duration;
    }

    std::optional<size_t> Schedule::next_green(size_t street_id, size_t current_time, std::optional<size_t> last_used_time) {
        if (green_lights_.contains(street_id)) {
            auto &&green_light_range = green_lights_.at(street_id);
            if (last_used_time.has_value()) {
                if (current_time == *last_used_time) {
                    ++current_time;
                }
            }
            auto time_normalized = current_time % duration_;
            auto start = green_light_range.begin();
            if (time_normalized < start) {
                return (current_time - time_normalized) + start;
            }
            auto end = green_light_range.end();
            if (time_normalized >= end) {
                return (current_time - time_normalized) + duration_ + start;
            }
            return current_time;
        }
        return {};
    }

    void Schedule::reset() {
        duration_ = 0;
        green_lights_.clear();
    }

}
