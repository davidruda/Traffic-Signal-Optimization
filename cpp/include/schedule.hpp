#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>

#include "street.hpp"

class Schedule {
public:
    explicit Schedule(int intersection_id);
    int duration() const;
    void add_street(int street_id, int green_light_duration);
    std::optional<int> next_green(int time, const Street::Instance &street);

private:
    int intersection_id_;
    int duration_;
    int last_used_time_;
    std::unordered_map<int, std::ranges::iota_view<int, int>> green_light_ranges_;
};


#endif
