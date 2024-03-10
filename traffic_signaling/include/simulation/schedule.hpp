#ifndef SIMULATION_SCHEDULE_HPP
#define SIMULATION_SCHEDULE_HPP

#include <optional>
#include <unordered_map>
#include <vector>
#include <ranges>

// TODO: add comments with explanations of the datastructures (they're complex)

namespace simulation {
class Schedule {
public:
    Schedule() = default;

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

    void add_street(unsigned long street_id, unsigned long green_light_duration);
    std::optional<unsigned long> next_green(unsigned long street_id, unsigned long time) const;

    std::pair<std::vector<unsigned long>, std::vector<unsigned long>> get() const;

    void set(const std::vector<unsigned long> &order, const std::vector<unsigned long> &times);

    void reset();

private:
    using TimeInterval = std::ranges::iota_view<unsigned long, unsigned long>;

    unsigned long total_duration_{};

    // order of the street_ids in the green light schedule
    std::vector<unsigned long> order_;
    std::vector<unsigned long> times_;
    // green light intervals indexed by street_id 
    std::unordered_map<unsigned long, TimeInterval> green_lights_;
};
}

#endif
