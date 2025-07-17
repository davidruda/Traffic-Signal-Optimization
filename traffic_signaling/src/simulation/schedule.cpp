#include <algorithm>
#include <cassert>
#include <stdexcept>

#include "simulation/schedule.hpp"
#include "city_plan/street.hpp"

namespace simulation {

namespace {
    std::mt19937_64 random_engine{42};
}

unsigned long Schedule::divisor_ = DEFAULT_DIVISOR;

void set_seed(unsigned long seed) {
    random_engine.seed(seed);
}

Schedule::Schedule(
    const city_plan::Intersection &intersection,
    std::vector<unsigned long> &&order, std::vector<unsigned long> &&times, bool relative_order
) : intersection_(intersection) {
    set(std::move(order), std::move(times), relative_order);
}

std::optional<unsigned long> Schedule::next_green(unsigned long street_id, unsigned long time) {
    if (adaptive_  && !green_lights_.contains(street_id)) {
        add_street_adaptive(street_id, time);
    }
    if (!green_lights_.contains(street_id)) {
        return {};
    }
    auto &&green_light = green_lights_.at(street_id);
    // if the duration of the green light is zero
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

void Schedule::set(std::vector<unsigned long> &&order, std::vector<unsigned long> &&times, bool relative_order) {
    assert(order.size() == times.size());
    reset();
    if (relative_order) {
        auto street_index_to_street_id = [&](unsigned long &street_index) {
            const city_plan::Street &street = intersection_.used_streets()[street_index];
            street_index = street.id();
        };
        // convert relative order street indices to street ids
        std::ranges::for_each(order, street_index_to_street_id);
    }
    for (size_t i = 0; i < order.size(); ++i) {
        green_lights_.try_emplace(order[i], total_duration_, total_duration_ + times[i]);
        total_duration_ += times[i];
    }
    order_ = std::move(order);
    times_ = std::move(times);
}

void Schedule::set(Order order_type, Times times_type) {
    // adaptive schedules must be handled separately
    if (order_type == Order::ADAPTIVE) {
        set_adaptive();
        return;
    }

    std::vector<unsigned long> order, times;
    auto &&street_ids = intersection_.used_streets() | std::views::transform(&city_plan::Street::id);
    // DEFAULT order is the order of the streets in the intersection
    order.assign(street_ids.begin(), street_ids.end());

    if (order_type == Order::RANDOM) {
        // Use our own shuffle function instead of std::shuffle for more consistent results
        deterministic_shuffle(order.begin(), order.end(), random_engine);
    }

    if (times_type == Times::DEFAULT) {
        times.assign(order.size(), 1UL);
    }
    else if (times_type == Times::SCALED) {
        auto &&car_counts = order | std::views::transform([&](unsigned long street_id) {
            return intersection_.used_streets()[intersection_.street_index(street_id)].get().total_cars();
        });
        auto scale = [](unsigned long car_count) {
            return std::max(car_count / divisor_, 1UL);
        };
        auto &&scaled_car_counts = car_counts | std::views::transform(scale);
        times.assign(scaled_car_counts.begin(), scaled_car_counts.end());
    }

    set(std::move(order), std::move(times));
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
