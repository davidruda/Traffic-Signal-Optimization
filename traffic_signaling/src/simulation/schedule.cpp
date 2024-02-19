#include <algorithm>
#include <vcruntime.h>

#include "simulation/schedule.hpp"

// TODO: add comments with explanations of the complicated parts

namespace simulation {

void Schedule::add_street(size_t street_id, size_t green_light_duration) {
    order_.push_back(street_id);
    green_lights_.try_emplace(street_id, total_duration_, total_duration_ + green_light_duration);
    total_duration_ += green_light_duration;
}

std::optional<size_t> Schedule::next_green(size_t street_id, size_t time) const {
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

std::pair<std::vector<size_t>, std::vector<size_t>> Schedule::get() const {
    std::vector<size_t> times;
    times.reserve(order_.size());
    for (auto &&street_id: order_) {
        times.push_back(green_lights_.at(street_id).size());
    }
    return std::make_pair(order_, std::move(times));
}

void Schedule::set(const std::vector<size_t> &order, const std::vector<size_t> &times) {
    assert(order.size() == times.size());
    reset();
    for (size_t i = 0; i < order.size(); ++i) {
        add_street(order[i], times[i]);
    }
}

void Schedule::reset() {
    total_duration_ = 0;
    order_ = {};
    green_lights_ = {};
}
}
