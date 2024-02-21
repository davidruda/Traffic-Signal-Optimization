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

    size_t length() const {
        return green_lights_.size();
    }

    size_t duration() const {
        return total_duration_;
    }

    void add_street(size_t street_id, size_t green_light_duration);
    std::optional<size_t> next_green(size_t street_id, size_t time) const;

    std::pair<std::vector<size_t>, std::vector<size_t>> get() const;

    void set(const std::vector<size_t> &order, const std::vector<size_t> &times);

    void reset();

private:
    size_t total_duration_{};

    // order of the street_ids in the green light schedule
    std::vector<size_t> order_;
    // green light intervals indexed by street_id 
    std::unordered_map<size_t, std::ranges::iota_view<size_t, size_t>> green_lights_;
};
}

#endif
