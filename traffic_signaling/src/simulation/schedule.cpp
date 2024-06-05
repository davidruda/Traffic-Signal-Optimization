#include <cassert>
#include <stdexcept>

#include "simulation/schedule.hpp"
#include "city_plan/street.hpp"

// TODO: add comments with explanations of the complicated parts

namespace simulation {

Schedule::Schedule(const city_plan::Intersection &intersection, std::string_view option)
    : intersection_(intersection) {
    if (option == "default") {
        set_default();
        return;
    }
    if (option == "adaptive") {
        set_adaptive();
        return;
    }
    throw std::runtime_error{"Invalid option for Schedule"};
}

Schedule::Schedule(
    const city_plan::Intersection &intersection,
    std::vector<unsigned long> &&order, std::vector<unsigned long> &&times
) : intersection_(intersection) {
    set(std::move(order), std::move(times));
}

std::optional<unsigned long> Schedule::next_green(unsigned long street_id, unsigned long time) {
    if (adaptive_  && !green_lights_.contains(street_id)) {
        add_street_adaptive(street_id, time);
    }
    if (!green_lights_.contains(street_id)) {
        return {};
    }
    auto &&green_light = green_lights_.at(street_id);
    // if duration of the green light is zero
    if (green_light.empty()) {
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

void Schedule::fill_missing_streets() {
    // Filling the missing streets could be done in a more efficient way, but it's probably not necessary
    for (const city_plan::Street &street: intersection_.used_streets()) {
        if (!green_lights_.contains(street.id())) {
            add_street_adaptive(street.id(), 0);
        }
    }
}

void Schedule::add_street(unsigned long street_id, unsigned long green_light_duration) {
    order_.push_back(street_id);
    times_.push_back(green_light_duration);
    green_lights_.try_emplace(street_id, total_duration_, total_duration_ + green_light_duration);
    total_duration_ += green_light_duration;
}

void Schedule::add_street_adaptive(unsigned long street_id, unsigned long time) {
    time %= total_duration_;
    for (auto i = 0UL; i < total_duration_; ++i) {
        // find the first unused slot
        auto t = (time + i) % total_duration_;
        if (order_[t] == UNUSED) {
            order_[t] = street_id;
            green_lights_.try_emplace(street_id, t, t + 1);
            return;
        }
    }
    throw std::runtime_error{"No unused slot found"};
}

void Schedule::set(const std::vector<unsigned long> &order, const std::vector<unsigned long> &times) {
    assert(order.size() == times.size());
    reset();
    for (size_t i = 0; i < order.size(); ++i) {
        add_street(order[i], times[i]);
    }
}

void Schedule::set(std::vector<unsigned long> &&order, std::vector<unsigned long> &&times) {
    assert(order.size() == times.size());
    reset();
    for (size_t i = 0; i < order.size(); ++i) {
        green_lights_.try_emplace(order[i], total_duration_, total_duration_ + times[i]);
        total_duration_ += times[i];
    }
    order_ = std::move(order);
    times_ = std::move(times);
}

void Schedule::set_default() {
    reset();
    for (const city_plan::Street &street: intersection_.used_streets()) {
        add_street(street.id(), 1);
    }
}

void Schedule::set_adaptive() {
    reset();
    adaptive_ = true;
    total_duration_ = static_cast<unsigned long>(intersection_.used_streets().size());
    // initialize order with UNUSED since the order is determined adaptively
    order_.resize(total_duration_, UNUSED);
    // initialize times with 1 second for every street
    times_.resize(total_duration_, 1);
    green_lights_.reserve(total_duration_);
}

void Schedule::reset() {
    adaptive_ = {};
    total_duration_ = {};
    order_ = {};
    times_ = {};
    green_lights_ = {};
}
}
