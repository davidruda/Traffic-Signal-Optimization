#include <algorithm>

#include "simulation/schedule.hpp"

// TODO: add comments with explanations of the complicated parts

namespace simulation {

void TimeInterval::set_interval(size_t begin, size_t end) {
    assert(begin <= end);
    begin_ = begin;
    end_ = end;
}

std::vector<std::pair<size_t, TimeInterval>> Schedule::green_lights() const {
    std::unordered_map<size_t, size_t> inverted_index;
    inverted_index.reserve(street_index_.size());
    for (auto &&[street_id, index]: street_index_) {
        inverted_index.emplace(index, street_id);
    }
    std::vector<std::pair<size_t, TimeInterval>> green_lights;
    green_lights.reserve(green_lights_.size());
    for (size_t i = 0; i < green_lights_.size(); ++i) {
        green_lights.emplace_back(inverted_index[i], green_lights_[i]);
    }

    // Sort by start time of each street's green light
    auto cmp = [](const auto &l, const auto &r) {
        return l.second.begin() <= r.second.begin();
    };
    std::sort(green_lights.begin(), green_lights.end(), cmp);

    return green_lights;
}

void Schedule::add_street(size_t street_id, size_t green_light_duration) {
    if (!street_index_.contains(street_id)) {
        street_index_[street_id] = green_lights_.size();
        green_lights_.emplace_back(total_duration_, total_duration_ + green_light_duration);
    }
    else {
        green_lights_[street_index_[street_id]] = TimeInterval(total_duration_, total_duration_ + green_light_duration);
    }
    total_duration_ += green_light_duration;
}

std::optional<size_t> Schedule::next_green(size_t street_id, size_t time) const {
    if (!street_index_.contains(street_id)) {
        return {};
    }
    auto &&green_light = green_lights_[street_index_.at(street_id)];
    if (green_light.duration() <= 0) {
        return {};
    }
    auto time_normalized = time % total_duration_;
    auto start = green_light.begin();
    if (time_normalized < start) {
        return (time - time_normalized) + start;
    }
    auto end = green_light.end();
    if (time_normalized >= end) {
        return (time - time_normalized) + total_duration_ + start;
    }
    return time;
}

std::pair<std::vector<size_t>, std::vector<size_t>> Schedule::get_schedule() const {
    std::vector<size_t> times, order;
    times.reserve(green_lights_.size());
    order.reserve(green_lights_.size());
    for (size_t i = 0; i < green_lights_.size(); ++i) {
        times.push_back(green_lights_[i].duration());
        order.push_back(i);
    }

    return std::make_pair(std::move(times), std::move(order));
}

void Schedule::set_schedule(const std::vector<size_t> &times, const std::vector<size_t> &order) {
    assert(order.size() == times.size());
    total_duration_ = 0;
    for (auto &&index: order) {
        green_lights_[index].set_interval(total_duration_, total_duration_ + times[index]);
        total_duration_ += times[index];
    }
}

void Schedule::reset() {
    total_duration_ = 0;
    street_index_ = {};
    green_lights_ = {};
}
}
