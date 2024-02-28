#ifndef CITY_PLAN_CAR_HPP
#define CITY_PLAN_CAR_HPP

#include <vector>
#include <functional>

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

private:
    size_t id_;
    std::vector<std::reference_wrapper<const Street>> path_;
};
}

#endif
