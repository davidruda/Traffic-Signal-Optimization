#ifndef SIMULATION_CAR_HPP
#define SIMULATION_CAR_HPP

#include <optional>

#include "city_plan/car.hpp"

namespace simulation {
/**
 * Car in the simulation.
 *
 * Contains the "dynamic" data and manipulates the car during the simulation.
 */
class Car {
public:
    /**
     * Construct a car object for the simulation.
     *
     * @param data "Static" information of the car from the city plan.
     */
    explicit Car(const city_plan::Car &data)
        : data_(data) {}

    /**
     * Return the ID of the street the car is currently on.
     */
    unsigned long current_street() const {
        return data_.path()[path_index_].get().id();
    }

    /**
     * Move the car to the next street in its path.
     */
    void move_to_next_street() {
        ++path_index_;
    }

    /**
     * Return True if the car is at the final street in its path.
     */
    bool final_destination() const {
        return data_.path().size() - 1 == path_index_;
    }

    /**
     * Return the ID of the car.
     */
    unsigned long id() const {
        return data_.id();
    }

    /**
     * Mark the car as having arrived at its destination.
     *
     * @param arrival_time Time the car arrived at its destination.
     * @param max_time Duration of the simulation.
     * @param bonus Bonus for arriving before the end of the simulation.
     */
    void arrive(unsigned long arrival_time, unsigned long max_time, unsigned long bonus) {
        arrival_time_ = arrival_time;
        score_ = bonus + max_time - arrival_time;
    }

    /**
     * Return the time the car arrived at its destination.
     */
    std::optional<unsigned long> arrival_time() const {
        return arrival_time_;
    }

    /**
     * Return the score of the car.
     */
    unsigned long score() const {
        return score_;
    }

    /**
     * Reset the car to its initial state.
     *
     * This method is used for performance reasons to avoid frequent reallocation.
     */
    void reset() {
        path_index_ = {};
        arrival_time_ = {};
        score_ = {};
    }

private:
    /** "Static" information of the car from the city plan. */
    const city_plan::Car &data_;

    /** Index of the street the car is currently on in its path. */
    unsigned long path_index_{};
    /** Time the car arrived at its destination, if it has arrived. */
    std::optional<unsigned long> arrival_time_;
    /** Score of the car after running the simulation. */
    unsigned long score_{};
};
}

#endif
