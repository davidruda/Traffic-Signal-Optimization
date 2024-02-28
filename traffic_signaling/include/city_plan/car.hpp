#ifndef CITY_PLAN_CAR_HPP
#define CITY_PLAN_CAR_HPP

#include <vector>
#include <functional>
#include <ranges>

#include "city_plan/street.hpp"

namespace city_plan {
class Car {
public:
    Car(size_t id, std::vector<std::reference_wrapper<const Street>> &&path)
        : id_(id), path_(std::move(path)) {}

    size_t id() const {
        return id_;
    }

    const std::vector<std::reference_wrapper<const Street>> &path() const {
        return path_;
    }

    size_t path_duration() const {
        size_t duration = 0;
        // The car path begins at the end of the first street
        // so we skip the first street
        for (const Street &s: path_ | std::views::drop(1)) {
            duration += s.length();
        }
        return duration;
    }

private:
    size_t id_;
    std::vector<std::reference_wrapper<const Street>> path_;
};
}

#endif
