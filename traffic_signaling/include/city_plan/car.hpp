#ifndef CITY_PLAN_CAR_HPP
#define CITY_PLAN_CAR_HPP

#include <vector>
#include <functional>
#include <ranges>

#include "city_plan/street.hpp"

namespace city_plan {
/**
 * Car in the city plan.
 *
 * Contains all "static" information about a car known from the input data.
 */
class Car {
public:
    /**
     * Construct a car object.
     *
     * @param id ID of the car.
     * @param path Path of streets the car has to travel.
     */
    Car(unsigned long id, std::vector<std::reference_wrapper<const Street>> &&path)
        : id_(id), path_(std::move(path)) {}

    /**
     * Return the ID of the car.
     */
    unsigned long id() const {
        return id_;
    }

    /**
     * Return the path the car has to travel.
     */
    const std::vector<std::reference_wrapper<const Street>> &path() const {
        return path_;
    }

    /**
     * Return the duration in seconds to travel the car's path without any waiting at traffic lights.
     */
    unsigned long path_duration() const {
        unsigned long duration = 0;
        // The car path begins at the end of the first street
        // so we skip the first street
        for (const Street &s: path_ | std::views::drop(1)) {
            duration += s.length();
        }
        return duration;
    }

private:
    /** ID of the car. */
    unsigned long id_;
    /** Path of streets the car has to travel. */
    std::vector<std::reference_wrapper<const Street>> path_;
};
}

#endif
