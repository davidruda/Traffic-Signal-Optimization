#include <cassert>

#include "simulation/schedule.hpp"

// TODO: add comments with explanations of the complicated parts

namespace simulation {

void Schedule::add_street(unsigned long street_id, unsigned long green_light_duration) {
    order_.push_back(street_id);
    times_.push_back(green_light_duration);
    green_lights_.try_emplace(street_id, total_duration_, total_duration_ + green_light_duration);
    total_duration_ += green_light_duration;
}

std::optional<unsigned long> Schedule::next_green(unsigned long street_id, unsigned long time) const {
    if (!green_lights_.contains(street_id)) {
        return {};
    }
    auto &&green_light = green_lights_.at(street_id);
    // if duration of the green light is zero
    if (green_light.size() <= 0) {
        return {};
    }
    auto time_normalized = time % total_duration_;
    auto start = *green_light.begin();
    if (time_normalized < start) {
        return (time - time_normalized) + start;
    }
    auto end = *green_light.end();
    if (time_normalized >= end) {
        return (time - time_normalized) + total_duration_ + start;
    }
    return time;
}

void Schedule::set(const std::vector<unsigned long> &order, const std::vector<unsigned long> &times) {
    assert(order.size() == times.size());
    reset();
    for (size_t i = 0; i < order.size(); ++i) {
        add_street(order[i], times[i]);
    }
}

void Schedule::reset() {
    total_duration_ = {};
    order_ = {};
    times_ = {};
    green_lights_ = {};
}
}
