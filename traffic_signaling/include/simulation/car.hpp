#ifndef SIMULATION_CAR_HPP
#define SIMULATION_CAR_HPP

#include <optional>

#include "city_plan/car.hpp"

namespace simulation {
class Car {
public:
    explicit Car(const city_plan::Car &data)
        : data_(data) {}

    size_t current_street() const {
        return data_.path()[path_index_].get().id();
    }

    void move_to_next_street() {
        ++path_index_;
    }

    bool final_destination() const {
        return data_.path().size() - 1 == path_index_;
    }

    size_t id() const {
        return data_.id();
    }

    void arrive(size_t arrival_time, size_t max_time, size_t bonus) {
        arrival_time_ = arrival_time;
        score_ = bonus + max_time - arrival_time;
    }

    std::optional<size_t> arrival_time() const {
        return arrival_time_;
    }

    size_t score() const {
        return score_;
    }

    void reset() {
        path_index_ = {};
        arrival_time_ = {};
        score_ = {};
    }

private:
    const city_plan::Car &data_;

    size_t path_index_{};
    std::optional<size_t> arrival_time_;
    size_t score_{};
};
}

#endif
