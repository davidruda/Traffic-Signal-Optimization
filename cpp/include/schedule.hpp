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
    size_t length() const;
    int duration() const;
    void add_street(int street_id, int green_light_duration);
    std::optional<int> next_green(int time, const Street::Instance &street);

    using iterator = typename std::unordered_map<int, std::ranges::iota_view<int, int>>::iterator;
    using const_iterator = typename std::unordered_map<int, std::ranges::iota_view<int, int>>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    int intersection_id_;
    int duration_;
    std::unordered_map<int, std::ranges::iota_view<int, int>> green_light_ranges_;
};


#endif
