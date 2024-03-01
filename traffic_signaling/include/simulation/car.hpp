#ifndef SIMULATION_CAR_HPP
#define SIMULATION_CAR_HPP

#include <optional>

#include "city_plan/car.hpp"

namespace simulation {
class Car {
public:
    explicit Car(const city_plan::Car &data)
        : data_(data) {}

    unsigned long current_street() const {
        return data_.path()[path_index_].get().id();
    }

    void move_to_next_street() {
        ++path_index_;
    }

    bool final_destination() const {
        return data_.path().size() - 1 == path_index_;
    }

    unsigned long id() const {
        return data_.id();
    }

    void arrive(unsigned long arrival_time, unsigned long max_time, unsigned long bonus) {
        arrival_time_ = arrival_time;
        score_ = bonus + max_time - arrival_time;
    }

    std::optional<unsigned long> arrival_time() const {
        return arrival_time_;
    }

    unsigned long score() const {
        return score_;
    }

    void reset() {
        path_index_ = {};
        arrival_time_ = {};
        score_ = {};
    }

private:
    const city_plan::Car &data_;

    unsigned long path_index_{};
    std::optional<unsigned long> arrival_time_;
    unsigned long score_{};
};
}

#endif
