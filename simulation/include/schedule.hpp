#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>

#include "street.hpp"

class Schedule {
public:
    explicit Schedule(size_t intersection_id);
    size_t length() const;
    size_t duration() const;
    void add_street(size_t street_id, size_t green_light_duration);
    std::optional<size_t> next_green(const Street::Instance &street, size_t time);

private:
    size_t intersection_id_;
    size_t duration_;
    std::unordered_map<size_t, std::ranges::iota_view<size_t, size_t>> green_light_ranges_;
};


#endif
