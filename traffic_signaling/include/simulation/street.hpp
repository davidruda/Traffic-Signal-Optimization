#ifndef SIMULATION_STREET_HPP
#define SIMULATION_STREET_HPP

#include <optional>
#include <queue>

#include "city_plan/street.hpp"

namespace simulation {
/**
 * Street in the simulation.
 *
 * Contains the "dynamic" data and operates the street during the simulation.
 */
class Street {
public:
    /**
     * Construct a street object for the simulation.
     *
     * @param data "Static" information of the street from the city plan.
     */
    explicit Street(const city_plan::Street &data)
        : data_(data) {}

    /**
     * Add a car to the street's queue.
     *
     * @param car_id ID of the car.
     * @param time Time when the car receives the green light and can move to the next street.
     */
    void add_car(unsigned long car_id, unsigned long time) {
        car_queue_.emplace(car_id);
        latest_used_time_ = time;
    }

    /**
     * Pop the first car from the street's queue and return its ID.
     */
    unsigned long get_car() {
        auto &&car = car_queue_.front();
        car_queue_.pop();
        return car;
    }

    /**
     * Return the ID of the street.
     */
    unsigned long id() const {
        return data_.id();
    }

    /**
     * Return the latest time a car passed the traffic light on this street.
     */
    std::optional<unsigned long> latest_used_time() const {
        return latest_used_time_;
    }

    /**
     * Reset the street to its initial state.
     *
     * This method is used for performance reasons to avoid frequent reallocation.
     */
    void reset() {
        car_queue_ = {};
        latest_used_time_ = {};
    }

private:
    /** "Static" information of the street from the city plan. */
    const city_plan::Street &data_;

    /** Queue of cars (represented by their IDs) waiting to pass the traffic light on this street. */
    std::queue<unsigned long> car_queue_;
    /** The latest time a car passed the traffic light on this street. */
    std::optional<unsigned long> latest_used_time_;
};
}

#endif
