#ifndef SIMULATION_SCHEDULE_HPP
#define SIMULATION_SCHEDULE_HPP

#include <optional>
#include <unordered_map>
#include <vector>
#include <ranges>
#include <limits>
#include <string_view>

#include "city_plan/intersection.hpp"

// TODO: add comments with explanations of the datastructures (they're complex)

namespace simulation {
class Schedule {
public:
    explicit Schedule(const city_plan::Intersection &intersection, std::string_view option = "default");
    Schedule(
        const city_plan::Intersection &intersection,
        std::vector<unsigned long> &&order, std::vector<unsigned long> &&times
    );

    unsigned long length() const {
        return static_cast<unsigned long>(green_lights_.size());
    }

    unsigned long duration() const {
        return total_duration_;
    }

    const std::vector<unsigned long> &order() const {
        return order_;
    }

    const std::vector<unsigned long> &times() const {
        return times_;
    }

    std::optional<unsigned long> next_green(unsigned long street_id, unsigned long time);

    std::pair<std::vector<unsigned long>, std::vector<unsigned long>> get() const {
        return std::make_pair(order_, times_);
    }

    void set(const std::vector<unsigned long> &order, const std::vector<unsigned long> &times);
    void set(std::vector<unsigned long> &&order, std::vector<unsigned long> &&times);
    void set_default();
    void set_adaptive();

    void reset();
    
    // helper function to fill in the missing streets when using adaptive schedule
    void fill_missing_streets();

private:
    using TimeInterval = std::ranges::iota_view<unsigned long, unsigned long>;

    void add_street(unsigned long street_id, unsigned long green_light_duration);
    void add_street_adaptive(unsigned long street_id, unsigned long time);

    const city_plan::Intersection &intersection_;
    bool adaptive_{};
    unsigned long total_duration_{};

    // order of the street_ids in the green light schedule
    std::vector<unsigned long> order_;
    std::vector<unsigned long> times_;
    // green light intervals indexed by street_id 
    std::unordered_map<unsigned long, TimeInterval> green_lights_;

    static constexpr auto UNUSED = std::numeric_limits<unsigned long>::max();
};
}

#endif
