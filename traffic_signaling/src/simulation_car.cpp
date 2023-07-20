#include "simulation_car.hpp"

namespace simulation {

    Car::Car(const city_plan::Car &data)
        : data_(data), path_index_(START), finish_time_(UNFINISHED) {}

    size_t Car::current_street() const {
        return data_.path()[path_index_];
    }

    void Car::move_to_next_street() {
        ++path_index_;
    }

    bool Car::final_destination() const {
        return data_.path().size() - 1 == path_index_;
    }

    void Car::set_finish_time(size_t finish_time) {
        finish_time_ = finish_time;
    }

    size_t Car::finish_time() const {
        return finish_time_;
    }

    bool Car::finished() const {
        return finish_time_ != UNFINISHED;
    }

    size_t Car::id() const {
        return data_.id();
    }

    void Car::reset() {
        path_index_ = START;
        finish_time_ = UNFINISHED;
    }

}
